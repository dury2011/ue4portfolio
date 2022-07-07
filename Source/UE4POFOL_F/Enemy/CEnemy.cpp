/* MEMO: CEnemy.cpp �ڵ� ��ġ ���� �Ϸ� */
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
#include "Weapon/CRifle.h"
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
	
	//TODO: ���� �ȵ�, �������Ʈ���� ���� ����ߵ�
	//if (HealthBarWidget)
	//	HealthBarWidgetComponent->SetWidgetClass(HealthBarWidget);
}

void ACEnemy::BeginPlay()
{
	Super::BeginPlay(); 

	// ���� ���ε�
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

	// ���� ����, ���� ���� ����
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

	// ����(Player) ����
	{
		TArray<AActor*> outActorArr;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPlayer::StaticClass(), outActorArr);

		for (int i = 0; i < outActorArr.Num(); i++)
			Opponent = dynamic_cast<ACPlayer*>(outActorArr[i]);
	}

	// HealthBar ���� Hidden ����
	{
		if (HealthBarWidgetComponent)
			HealthBarWidgetComponent->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
	}

	// Ÿ�̸� ����
	{
		//GetWorldTimerManager().SetTimer(StrafeTimer, this, &ACEnemy::ChangeStrafing, ChangeStrafingTypeInterval, true);
	}

	// ����ü ����
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
	
	// MEMO: Enemy�� Dead �����̸� ��� �׽�ũ ���� �������� ����Ǿ�� ��
	if (CurrentStateType == EEnemyStateType::Dead)
	{
		if (OnEnemyAttackEnded.IsBound())
			OnEnemyAttackEnded.Broadcast();

		if (OnEnemyParkourEnded.IsBound())
			OnEnemyParkourEnded.Broadcast();

		GLog->Log("ACEnemy::MontageEnded By EnemyDead");

		return;
	}

	UpdateHitNumbers();

	//
	//if (Controller)
	//{
	//	//UObject* obj = Controller->GetBlackboardComponent()->GetValueAsObject(FName("Player"));
	//	//Player = Cast<ACPlayer>(obj);
	//
	//	float yawDeg = FMath::DegreesToRadians(GetActorRotation().Yaw);
	//	float actorX = GetActorLocation().X;
	//	float actorY = GetActorLocation().Y;
	//	float x1 = GetActorLocation().X;
	//	float x2 = GetActorLocation().X;
	//	float y1 = GetActorLocation().Y + 100.f;
	//	float y2 = GetActorLocation().Y - 100.f;
	//
	//	FMatrix2x2 rotationMatrix2D(FMath::Cos(yawDeg), -(FMath::Sin(yawDeg)), FMath::Sin(yawDeg), FMath::Cos(yawDeg));
	//
	//	float cos00, mSin01, sin10, cos11;
	//
	//	rotationMatrix2D.GetMatrix(cos00, mSin01, sin10, cos11);
	//
	//	// rotationMatrix2D * (x1, y1)
	//	float newX1 = (cos00 * (x1 - actorX)) + (mSin01 * (y1 - actorY));
	//	float newY1 = (sin10 * (x1 - actorX)) + (cos11 * (y1 - actorY));
	//
	//	// rotationMatrix2D * (x2, y2)
	//	float newX2 = (cos00 * (x2 - actorX)) + (mSin01 * (y2 - actorY));
	//	float newY2 = (sin10 * (x2 - actorX)) + (cos11 * (y2 - actorY));
	//	
	//	if (WeaponComponent->IsRifleMode() || WeaponComponent->IsOnehandMode())
	//	{	
	//		WorldAttackPoint = FVector(Pos.X + newX1, Pos.Y + newY1, Pos.Z);
	//		WorldAttackPoint2 = FVector(Pos.X + newX2, Pos.Y + newY2, Pos.Z);
	//	
	//		Controller->GetBlackboardComponent()->SetValueAsVector(TEXT("AttackPoint"), WorldAttackPoint);
	//		Controller->GetBlackboardComponent()->SetValueAsVector(TEXT("AttackPoint2"), WorldAttackPoint2);
	//	}
	//	else
	//	{
	//		WorldAttackPoint = FVector(GetActorLocation().X + newX1, GetActorLocation().Y + newY1, GetActorLocation().Z);
	//		WorldAttackPoint2 = FVector(GetActorLocation().X + newX2, GetActorLocation().Y + newY2, GetActorLocation().Z);
	//	}
	//}

#ifdef DEBUG_CENEMY
	DrawDebugSphere(GetWorld(), WorldAttackPoint, 25.f, 12, FColor::Red, false);
	DrawDebugSphere(GetWorld(), WorldAttackPoint2, 25.f, 12, FColor::Red, false);
#endif DEBUG_CENEMY
}

float ACEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	// Check Execute function body
	{
		if (CurrentStateType == EEnemyStateType::Dead)
			return 0.0f;

		//TODO: �ӽ� ���߿� Tag�� �ٲ� ���� 
		if (EventInstigator != GetWorld()->GetFirstPlayerController())
			return DamageAmount;
	}

	// ����ü �� �Ҵ� �� ü�� ���� 
	{
		Damaged.DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		Damaged.EventInstigator = EventInstigator;
		Damaged.DamageCauser = DamageCauser;
		//Damaged.DamageEvent = /* (FActionDamageEvent*)&DamageEvent; */ // EXPLAIN: ���⼭ TakeDamage�� Character�� HitData�� �Ҵ��
		Hp -= Damaged.DamageAmount;
	}

	FVector start = GetActorLocation();
	FVector target = Damaged.EventInstigator->GetPawn()->GetActorLocation();

	FVector direction = target - start;
	direction.Normalize();

	FTransform transform;
	transform.SetLocation(GetActorLocation());

	SetActorRotation(FRotator(GetActorRotation().Pitch, direction.Rotation().Yaw, GetActorRotation().Roll)/*UKismetMathLibrary::FindLookAtRotation(start, target)*/);
	LaunchCharacter(-direction * DeadDatas[0].Launch, true, false);

	ShowHitNumber(DamageAmount, this->GetActorLocation());
	ShowHealthBar();
	ShakeCamera(Damaged);
	//TODO: HitStop �������̽��� ���� ����?
	// HitStop
	{
		CustomTimeDilation = 10e-5f;

		GetWorldTimerManager().SetTimer(StopTimer, this, &ACEnemy::RecoverDilation, StopTime, true);
	}
	
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
	InHitNumber->RemoveFromParent(); // ����Ʈ���� ������ ������ ���̴�.
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
	// MEMO: Enemy�� ��Ÿ�ְ� ��� �� ���������� ����� ��� ��尡 �������� ��������
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
	// MEMO: ��Ÿ�� ��� ���� �ٸ� ��Ÿ�ְ� ����� ��� Interrupted == true
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
