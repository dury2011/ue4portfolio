/* MEMO: CEnemy.cpp 코드 배치 정리 완료 */
#include "Enemy/CEnemy.h"
#include "Global.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimInstance.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Weapon/CWeaponStructure.h"
#include "Animation/AnimMontage.h"
#include "Blueprint/UserWidget.h"
#include "Player/CPlayer.h"
#include "AI/CAIController.h"
#include "Kismet/GameplayStatics.h"
//#include "Weapon/CRifle.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Component/CCharacterComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetTextLibrary.h"
#include "Weapon/CWeapon.h"

//#define DEBUG_CENEMY

ACEnemy::ACEnemy()
	:HealthBarDisplayTime(2.f)
{
	RootComponent = GetCapsuleComponent();
	
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	CurrentStateType = EEnemyStateType::IdleOrJustMoving;
	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("Health Bar Widget");
	HealthBarWidgetComponent->SetupAttachment(GetMesh());
	//HealthBarWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f));
	HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidgetComponent->SetDrawSize(FVector2D(125.0f, 15.0f));
	
	//TODO: 적용 안됨, 블루프린트에서 직접 해줘야됨
	//if (HealthBarWidget)
	//	HealthBarWidgetComponent->SetWidgetClass(HealthBarWidget);
}

void ACEnemy::BeginPlay()
{
	Super::BeginPlay(); 

	// 각종 바인딩
	{
		GetComponents<UCapsuleComponent>(CapsuleCollisions);

		for (UShapeComponent* collision : CapsuleCollisions)
		{
			collision->OnComponentBeginOverlap.AddDynamic(this, &ACEnemy::OnBeginOverlap);
			collision->OnComponentEndOverlap.AddDynamic(this, &ACEnemy::OnEndOverlap);
			collision->OnComponentHit.AddDynamic(this, &ACEnemy::OnHit);
		}
	
		if (GetMesh()->GetAnimInstance())
			GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &ACEnemy::OnMontageEnded);
	}

	// 무기 스폰, 무기 오너 설정
	{
		FActorSpawnParameters params;
		params.Owner = this;

		for (int i = 0; i < WeaponClass.Num(); i++)
		{
			if (WeaponClass[i])
				Weapons.Add(GetWorld()->SpawnActor<ACWeapon>(WeaponClass[i], params));

			if (Weapons[i])
				Weapons[i]->SetOwner(this);
		}
	}

	// 상대방(Player) 설정
	{
		TArray<AActor*> outActorArr;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPlayer::StaticClass(), outActorArr);

		for (int i = 0; i < outActorArr.Num(); i++)
			Opponent = dynamic_cast<ACPlayer*>(outActorArr[i]);
	}

	if (Opponent)
	{
		Cast<ACPlayer>(Opponent)->OnPlayerSkillAttack.AddDynamic(this, &ACEnemy::TakeDamage_OpponentUsingSkill);
	}

	// HealthBar 위젯 Hidden 설정
	{
		if (HealthBarWidgetComponent)
			HealthBarWidgetComponent->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
	}

	// 타이머 설정
	{
		//GetWorldTimerManager().SetTimer(StrafeTimer, this, &ACEnemy::ChangeStrafing, ChangeStrafingTypeInterval, true);
	}

	// 열거체 설정
	{
		SetCurrentEnemyStateType(EEnemyStateType::IdleOrJustMoving);
	}
}

void ACEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (Opponent && bActivateRotateToOpponent)
	{
		CheckTrue(CurrentStateType == EEnemyStateType::Dead);
		
		FVector directionToOpponent = Opponent->GetActorLocation() - GetActorLocation();
		directionToOpponent.Z = 0.0f;
		FRotator targetRotation = FRotationMatrix::MakeFromX(directionToOpponent).Rotator();

		SetActorRotation(FMath::RInterpTo(GetActorRotation(), targetRotation, DeltaTime, RotationSpeed));
	}

	if (Opponent)
	{
		DistanceToOpponent = GetDistanceTo(this);
	}

	if (CanStrafing)
	{
		AddMovementInput(StrafeDirection);
	}

	if (IsLaunchBySkill)
	{
		SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, Opponent->GetActorLocation().Z));
	}


	UpdateHitNumbers();

#ifdef DEBUG_CENEMY
	DrawDebugSphere(GetWorld(), WorldAttackPoint, 25.f, 12, FColor::Red, false);
	DrawDebugSphere(GetWorld(), WorldAttackPoint2, 25.f, 12, FColor::Red, false);
#endif DEBUG_CENEMY
}

float ACEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	// 몸체 실행 전 조건 확인
	{
		if (CurrentStateType == EEnemyStateType::Dead)
			return 0.0f;

		//TODO: 임시 나중에 Tag로 바꿀 예정 
		if (EventInstigator != GetWorld()->GetFirstPlayerController())
			return DamageAmount;
	}

	// 구조체 값 할당 및 체력 빼기 
	{
		Damaged.DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		Damaged.EventInstigator = EventInstigator;
		Damaged.DamageCauser = DamageCauser;
		//Damaged.DamageEvent = /* (FActionDamageEvent*)&DamageEvent; */ // EXPLAIN: 여기서 TakeDamage할 Character의 HitData가 할당됨
		Hp -= Damaged.DamageAmount;
	}

	// Enemy 피격시 회전과 넉백 설정
	{
		FVector start = GetActorLocation();
		FVector target = Damaged.EventInstigator->GetPawn()->GetActorLocation();

		FVector direction = target - start;
		direction.Normalize();

		FTransform transform;
		transform.SetLocation(GetActorLocation());

		SetActorRotation(FRotator(GetActorRotation().Pitch, direction.Rotation().Yaw, GetActorRotation().Roll)/*UKismetMathLibrary::FindLookAtRotation(start, target)*/);
		LaunchCharacter(-direction * DeadDatas[0].Launch, true, false);
	}

	// Widget 각종 효과
	{
		ShowHitNumber(DamageAmount, this->GetActorLocation());
		ShowHealthBar();
		ShakeCamera(Damaged);
	}
	
	//OpponentSkillState();

	Damage();
	Dead();

	return Damaged.DamageAmount;
}

void ACEnemy::Damage()
{
	for (int i = 0; i < DamageDatas.Num(); i++)
		CheckFalse(DamageDatas[i].Montage);
	
	//CheckTrue(CurrentStateType == EEnemyStateType::Damage);

	if (Hp > 0.0f)
	{
		//SetCurrentEnemyStateType(EEnemyStateType::Damage);
		
		if (DamageDatas[0].Effect)
			DamageDatas[0].PlayEffect(GetWorld(), this);

		ActivateDamageEffect();

		//DamageDatas[0].PlayMontage(this);
	}
}

void ACEnemy::Dead()
{
	for (int i = 0; i < DeadDatas.Num(); i++)
		CheckFalse(DeadDatas[i].Montage);
	
	CheckTrue(CurrentStateType == EEnemyStateType::Dead);
	
	if (Hp <= 0.0f)
	{
		SetCurrentEnemyStateType(EEnemyStateType::Dead);
		
		for (int i = 0; i < Weapons.Num(); i++)
		{
			if (Weapons[i])
				Weapons[i]->DestroyWeapon();
		}

		DeadDatas[0].PlayMontage(this);

		ActivateDeadEffect();
	}
}

void ACEnemy::OpponentSkillState()
{
	if (Damaged.EventInstigator)
	{	
		ACharacter* character = Damaged.EventInstigator->GetCharacter();
		ICInterface_PlayerState* playerInterface = Cast<ICInterface_PlayerState>(character);

		if (playerInterface)
		{
		}
	}
}

void ACEnemy::OnAttack()
{
	CheckTrue(CurrentStateType == EEnemyStateType::Dead);

	for (int i = 0; i < ActionDatas.Num(); i++)
		CheckFalse(ActionDatas[i].Montage);

	CheckTrue(CurrentStateType == EEnemyStateType::Attack);
	SetCurrentEnemyStateType(EEnemyStateType::Attack);
}

void ACEnemy::ChangeStrafing()
{	
	CheckTrue(CurrentStateType == EEnemyStateType::Attack);
	CheckTrue(CurrentStateType == EEnemyStateType::Dead);

	typedef EEnemyStrafingType s;

	int32 select = UKismetMathLibrary::RandomIntegerInRange(0, 3);
	
	switch ((EEnemyStrafingType)select)
	{
		case s::Front :
		{
			//CurrentStrafingType = s::Front;
			//
			//StrafeDirection = GetActorForwardVector();
	
			break;
		}
		case s::Back:
		{
			//CurrentStrafingType = s::Back;
			//
			//StrafeDirection = GetActorForwardVector() * -1.0f;
	
			break;
		}
		case s::Left:
		{
			CurrentStrafingType = s::Left;
			
			StrafeDirection = GetActorRightVector() * -1.0f;
	
			break;
		}
		case s::Right:
		{
			CurrentStrafingType = s::Right;
	
			StrafeDirection = GetActorRightVector();
			
			break;
		}
		default:
		{
			StrafeDirection = FVector::ZeroVector;
	
			break;
		}
	}
}

void ACEnemy::BeginStrafing()
{
	CanStrafing = true;
}

void ACEnemy::EndStrafing()
{
	CanStrafing = false;
}

void ACEnemy::BeginDodge()
{
	CheckTrue(CurrentStateType == EEnemyStateType::Dead);

	for (int i = 0; i < DodgeDatas.Num(); i++)
		CheckFalse(DodgeDatas[i].Montage);

	CheckTrue(CurrentStateType == EEnemyStateType::Parkour);
	SetCurrentEnemyStateType(EEnemyStateType::Parkour);

	int select = UKismetMathLibrary::RandomIntegerInRange(0, 1);

	DodgeDatas[select].PlayMontage(this);
}

void ACEnemy::RecoverDilation()
{
	CustomTimeDilation = 1.0f;

	GetWorldTimerManager().ClearTimer(StopTimer);
}

void ACEnemy::ShakeCamera(FDamaged damage)
{
	APlayerController* controller = Cast<APlayerController>(damage.EventInstigator);
	
	if (controller && DamageCameraShakeClass)
		controller->PlayerCameraManager->StartCameraShake(DamageCameraShakeClass);
}

void ACEnemy::StoreHitNumber(UUserWidget* InHitNumber, FVector InLocation)
{
	HitNumbers.Add(InHitNumber, InLocation);

	FTimerHandle HitNumberTimer;
	FTimerDelegate HitNumberDelegate;
	HitNumberDelegate.BindUFunction(this, FName("DestroyHitNumber"), InHitNumber);
	GetWorld()->GetTimerManager().SetTimer(HitNumberTimer, HitNumberDelegate, HitNumberDestroyTime, false);
}

void ACEnemy::DestroyHitNumber(UUserWidget* InHitNumber)
{
	HitNumbers.Remove(InHitNumber);
	InHitNumber->RemoveFromParent(); // 뷰포트에서 위젯을 지워줄 것이다.
}

void ACEnemy::UpdateHitNumbers()
{
	for (auto& HitPair : HitNumbers)
	{
		UUserWidget* HitNumber{ HitPair.Key };
		const FVector Location{ HitPair.Value };
		FVector2D ScreenPosition;
		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), Location, ScreenPosition);
		HitNumber->SetPositionInViewport(ScreenPosition);
	}
}

void ACEnemy::ShowHealthBar()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(HealthBarTimer, this, &ACEnemy::HideHealthBar, HealthBarDisplayTime);

	if (HealthBarWidgetComponent)
		HealthBarWidgetComponent->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Visible);
}

void ACEnemy::HideHealthBar()
{
	if (HealthBarWidgetComponent)
		HealthBarWidgetComponent->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
}

void ACEnemy::SetCurrentEnemyStateType(EEnemyStateType InType)
{
	PreviousStateType = CurrentStateType;
	CurrentStateType = InType;

	if (OnEnemyStateTypeChanged.IsBound())
		OnEnemyStateTypeChanged.Broadcast(PreviousStateType, CurrentStateType);
}

void ACEnemy::SkillDamage()
{
	Hp -= 100.0f;

	// 피격 회전 
	{
		FVector start = GetActorLocation();
		FVector target = Opponent->GetActorLocation();

		FVector direction = target - start;
		direction.Normalize();

		FTransform transform;
		transform.SetLocation(GetActorLocation());

		SetActorRotation(FRotator(GetActorRotation().Pitch, direction.Rotation().Yaw, GetActorRotation().Roll)/*UKismetMathLibrary::FindLookAtRotation(start, target)*/);
		LaunchCharacter(-direction * 1.0f, true, false);
	}

	// Widget 각종 효과
	{
		ShowHitNumber(100.0f, this->GetActorLocation());
		ShowHealthBar();

		Damaged.EventInstigator = Opponent->GetController();
		ShakeCamera(Damaged);
	}
}

void ACEnemy::TakeDamage_OpponentUsingSkill()
{
	if (IsAttackBySkill)
	{
		ICInterface_PlayerState* playerInterface = Cast<ICInterface_PlayerState>(Opponent);

		if (playerInterface)
		{
			if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::W2_S)
			{		
				SkillDamage();
				
				IsLaunchBySkill = true;
				
				if (DamageDatas[0].Montage)
					DamageDatas[0].PlayMontage(this);

				if (DamageDatas[0].Effect)
					DamageDatas[0].PlayEffect(GetWorld(), this);
				
				FVector thisLocation = GetActorLocation();
				FVector opponentLocation = Opponent->GetActorLocation();

				SetActorLocation(FVector(thisLocation.X, thisLocation.Y, opponentLocation.Z));
				
			}
			else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::W2_E)
			{
				SkillDamage();
				
				if (DamageDatas[1].Montage)
					DamageDatas[1].PlayMontage(this);

				if (DamageDatas[1].Effect)
					DamageDatas[1].PlayEffect(GetWorld(), this);

				IsLaunchBySkill = false;
				IsAttackBySkill = false;
			}
			else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::WC_S)
			{
				SkillDamage();
				
				IsAttackBySkill = true;
				
				if (DamageDatas[1].Montage)
					DamageDatas[1].PlayMontage(this);

				if (DamageDatas[1].Effect)
					DamageDatas[1].PlayEffect(GetWorld(), this);
			}
			else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::WC_E)
			{
				SkillDamage();
				
				IsAttackBySkill = false;
			}
			else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::S3_S)
			{
				SkillDamage();
				
				LaunchCharacter(FVector(0.0f, 0.0f, 500.0f), false, false);

				if (DamageDatas[0].Montage)
					DamageDatas[0].PlayMontage(this);

				if (DamageDatas[0].Effect)
					DamageDatas[0].PlayEffect(GetWorld(), this);
			}
			else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::S3_E)
			{
				SkillDamage();

				if (DamageDatas[1].Montage)
					DamageDatas[1].PlayMontage(this);

				if (DamageDatas[1].Effect)
					DamageDatas[1].PlayEffect(GetWorld(), this);
			}

			//if (Hp > 0.0f)
			//{
			//	//ActivateDamageEffect();
			//}

			if (Hp <= 0.0f)
			{
				SetCurrentEnemyStateType(EEnemyStateType::Dead);

				IsDeadBySkill = true;

				for (int i = 0; i < Weapons.Num(); i++)
				{
					if (Weapons[i])
						Weapons[i]->DestroyWeapon();
				}
			}
		}
	}
}

void ACEnemy::OnStateTypeChange(EEnemyStateType InCurrentStateType)
{
	PreviousStateType = CurrentStateType;
	CurrentStateType = InCurrentStateType;

	if (OnEnemyStateTypeChanged.IsBound())
		OnEnemyStateTypeChanged.Broadcast(PreviousStateType, CurrentStateType);
}

void ACEnemy::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void ACEnemy::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ACEnemy::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}

void ACEnemy::OnMontageEnded(UAnimMontage* InMontage, bool Interrupted)
{
	//// MEMO: Enemy의 몽타주가 재생 후 정상적으로 종료될 경우 노드가 성공으로 끝나야함
	//if (CurrentStateType == EEnemyStateType::Dead)
	//{
	//	if (OnEnemyAttackEnded.IsBound())
	//		OnEnemyAttackEnded.Broadcast();
	//
	//	if (OnEnemyParkourEnded.IsBound())
	//		OnEnemyParkourEnded.Broadcast();
	//
	//	return;
	//}
	if (IsDeadBySkill)
	{
		ActivateDeadEffect();

		IsDeadBySkill = false;
	}

	if (!Interrupted)
	{
		if (CurrentStateType == EEnemyStateType::Attack)
		{
			if (OnEnemyAttackEnded.IsBound())
				OnEnemyAttackEnded.Broadcast();

			//SetCurrentEnemyStateType(EEnemyStateType::IdleOrJustMoving);

			GLog->Log("ACEnemy::OnMontageEnded Normal");

			return;
		}
		else if (CurrentStateType == EEnemyStateType::Parkour)
		{
			if (OnEnemyParkourEnded.IsBound())
				OnEnemyParkourEnded.Broadcast();

			//SetCurrentEnemyStateType(EEnemyStateType::IdleOrJustMoving);

			GLog->Log("ACEnemy::OnMontageEnded Normal");

			return;
		}
	}
	// MEMO: 몽타주 재생 도중 다른 몽타주가 재생될 경우 Interrupted == true
	else if (Interrupted)
	{
		GLog->Log("ACEnemy::OnMontageEnded Interrupted!");

		return;
	}
}

void ACEnemy::SpawnEnemy(AActor* InSpawner, TSubclassOf<ACEnemy> InSpawnEnemyClass)
{
	if (InSpawnEnemyClass)
	{
		FActorSpawnParameters params;
		params.Owner = InSpawner;
		
		InSpawner->GetWorld()->SpawnActor<ACEnemy>
		(
			InSpawnEnemyClass,
			InSpawner->GetActorTransform(),
			params
		);
	}
}

void ACEnemy::DestroyEnemy()
{
	Destroy();
}

void ACEnemy::SpawnEnemyEffectWeapon()
{
	for (int i = 0; i < EffectWeaponClass.Num(); i++)
		CheckFalse(EffectWeaponClass[i]);

	ACWeapon* effectWeapon = ACWeapon::SpawnWeapon(this, EffectWeaponClass[EffectWeaponIndex], GetMesh()->GetSocketLocation(EffectWeaponSpawnSocketName));
	
	if (effectWeapon)
	{
		effectWeapon->SetOwner(this);
		effectWeapon->SetActorRotation(GetActorRotation());
	}
}
