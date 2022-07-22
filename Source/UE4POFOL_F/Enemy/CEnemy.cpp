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
//#include "Weapon/CRifle.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Component/CCharacterComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetTextLibrary.h"
#include "Weapon/CWeapon.h"
#include "GameplayTagContainer.h"
#include "Components/SphereComponent.h"

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

	// ���� ��ų ���ݿ� ���� �ǰ� ���� �Լ� ���ε�
	if (Opponent)
	{
		Cast<ACPlayer>(Opponent)->OnPlayerSkillAttack.AddDynamic(this, &ACEnemy::TakeDamage_OpponentUsingSkill);
		Cast<ACPlayer>(Opponent)->OnPlayerSpellFistAttack.AddDynamic(this, &ACEnemy::TakeDamage_OpponentSpellFistAttack);
		Cast<ACPlayer>(Opponent)->OnPlayerSkillLaunch.AddDynamic(this, &ACEnemy::BoundUpBySkill);
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

	if (IsAttackBySkill && IsLaunchBySkill)
	{
		SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, Opponent->GetActorLocation().Z));
	}

	///* MEMO: Player�� Enemy�ǰ� �ݸ��� �����ȿ� ������ �� Ŭ������ IsAttackBySkill�� true�� �ǰ�
	// * ��Ƽ���̿����� ��������Ʈ�� ���ε� �� �Լ��� TakeDamage_~�Լ��� ȣ��ȴ�.
	// * ������ Player�� ��ų ��� ���� �ݸ��� ���� ������ Enemy�� ������ IsAttackBySkill�� false�� �Ǿ� 
	// * �ǰ� ���� ����� �ǰ� ��Ƽ������ ���� ���� ���޹��� ���� ��� ���� �� ä�� ���ְ� �Ǿ� �Ʒ� �ذ�å�� �ۼ��� 
	// */
	// Player Skill�� StopHit������ ���
	if (IsSkillStopHit && IsAttackBySkill)
	{
		ICInterface_PlayerState* playerInterface = Cast<ICInterface_PlayerState>(Opponent);

		if (playerInterface)
		{
			if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::FinalHit)
			{
				DeactivateDamageIceEffect();
				CustomTimeDilation = 1.0f;

				IsSkillStopHit = false;
			}
		}
	}

	// Player Skill�� BoundUp������ ���
	// Player Skill Weapon�� Tag�� "SFWeapon"���� ������ ��
	// Player Skill Weapon���� Follow�� SphereCollision�� Tag�� "Sphere_Following"���� ������ ��
	if (IsBoundUp && IsAttackBySkill)
	{
		TArray<AActor*> outWeaponArr;
		TArray<UActorComponent*> outSphereArr;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("SFWeapon"), outWeaponArr);

		for (int i = 0; i < outWeaponArr.Num(); i++)
			if (outWeaponArr[i])
				outSphereArr = Cast<ACWeapon>(outWeaponArr[i])->GetComponentsByTag(USphereComponent::StaticClass(), FName("Sphere_Following"));
		
		for (int i = 0; i < outSphereArr.Num(); i++)
			if (outSphereArr[i])
				SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, Cast<USceneComponent>(outSphereArr[i])->GetComponentLocation().Z));
	}

	UpdateHitNumbers();

#ifdef DEBUG_CENEMY
	DrawDebugSphere(GetWorld(), WorldAttackPoint, 25.f, 12, FColor::Red, false);
	DrawDebugSphere(GetWorld(), WorldAttackPoint2, 25.f, 12, FColor::Red, false);
#endif DEBUG_CENEMY
}


float ACEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	// ��ü ���� �� ���� Ȯ��
	{
		if (CurrentStateType == EEnemyStateType::Dead)
			return 0.0f;
	}
				
	// ����ü �� �Ҵ� �� ü�� ���� 
	{
		Damaged.DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		Damaged.EventInstigator = EventInstigator;
		Damaged.DamageCauser = DamageCauser;
		//Damaged.DamageEvent = /* (FActionDamageEvent*)&DamageEvent; */ // EXPLAIN: ���⼭ TakeDamage�� Character�� HitData�� �Ҵ��
		Hp -= Damaged.DamageAmount;
	}
	
	//// ��ų ��� �� �����Ǵ� ���⿡ ���� Ư�� �������� ��� �� �Լ� �������ͼ� ���� ���� ������ �Լ��� �б�
	//{
		//ICInterface_PlayerState* playerInterface = Cast<ICInterface_PlayerState>(Opponent);
		//
		//if (playerInterface)
		//{
		//	if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::SF1_S)
		//	{
		//		TakeDamage_OpponentSkillWeapon();
		//
		//		return 0.0f;
		//	}
		//	else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::SF2_E)
		//	{
		//		TakeDamage_OpponentSkillWeapon();
		//
		//		return 0.0f;
		//	}
		//}
	//}

	Dead();
	// Enemy �ǰݽ� ȸ���� �˹� ����
	{
		FVector start = GetActorLocation();
		FVector target = Damaged.EventInstigator->GetPawn()->GetActorLocation();

		FVector direction = target - start;
		direction.Normalize();

		FTransform transform;
		transform.SetLocation(GetActorLocation());

		SetActorRotation(FRotator(GetActorRotation().Pitch, direction.Rotation().Yaw, GetActorRotation().Roll)/*UKismetMathLibrary::FindLookAtRotation(start, target)*/);
		
		LaunchCharacter(-direction * 50.0f, true, false);
	}

	// Widget ���� ȿ��
	{
		ShowHitNumber(DamageAmount, this->GetActorLocation());
		ShowHealthBar();
		ShakeCamera(Damaged);
	}

	Damage();

	return Damaged.DamageAmount;
}

void ACEnemy::Damage()
{
	//for (int i = 0; i < DamageDatas.Num(); i++)
		//CheckFalse(DamageDatas[i].Montage);
	//
	//CheckTrue(CurrentStateType == EEnemyStateType::Damage);
	if (Hp > 0.0f)
	{
		//SetCurrentEnemyStateType(EEnemyStateType::Damage);
		//
		//if (DamageDatas[0].Effect)
		//	DamageDatas[0].PlayEffect(GetWorld(), this);
		//
		//
		//DamageDatas[0].PlayMontage(this);
		ActivateDamageEffect();
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

void ACEnemy::DamageBySkillAndActivateEffect()
{
	bActivateRotateToOpponent = false;
	
	// ������ ����
	float applyDamage = UKismetMathLibrary::RandomIntegerInRange(250, 300);
	
	Hp -= applyDamage;
	
	// �ǰ� ȸ�� 
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

	// �ǰ� ȿ�� 
	{
		PlaySkillDamageSound();
		
		// MEMO: �� �߰��� Damage Effect�� 5��° �迭 ���ҿ� �־� ��
		if (DamageDatas[5].Effect)
			DamageDatas[5].PlayEffect(GetWorld(), this);

		ShowHitNumber(applyDamage, this->GetActorLocation());
		ShowHealthBar();

		//Damaged.EventInstigator = Opponent->GetController();
		//ShakeCamera(Damaged);
	}
}

void ACEnemy::SkillDamageData(int32 InIndex)
{
	if (DamageDatas[InIndex].Montage)
		DamageDatas[InIndex].PlayMontage(this);

	if (DamageDatas[InIndex].Effect)
		DamageDatas[InIndex].PlayEffect(GetWorld(), this);
}

void ACEnemy::TakeDamage_OpponentUsingSkill()
{
	// MEMO: ���� IsAttackBySkill�� False�� ���� �ʾ� �ݸ��� ���� ���� Enemy�� TakeDamage_OpponentUsingSkill�� ȣ��Ǵ� ��� ���� (�ذ�)
	CheckTrue(CurrentStateType == EEnemyStateType::Dead);
	
	ICInterface_PlayerState* playerInterface = Cast<ICInterface_PlayerState>(Opponent);
	// MEMO: Player���� Skill�� ���ݴ��ϴ� ���̸�
	if (IsAttackBySkill && playerInterface)
	{
		//// MEMO: ��� EPlayerSkillType�� ��ų ���۰� ���� ���ϸ�Ǵµ� Effect ������ �� ��ȭ�Ͽ���
		//if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::W1_S)
		//{	
		//	SkillDamage();
		//	ExecSkillDamageData(0);
		//}
		//else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::W1_E)
		//{
		//	SkillDamage();
		//	ExecSkillDamageData(1);
		//
		//	IsAttackBySkill = false;
		//}		
		//if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::W2_S)
		//{		
		//	IsLaunchBySkill = true;
		//	
		//	SkillDamage();
		//	ExecSkillDamageData(0);
		//	
		//	FVector thisLocation = GetActorLocation();
		//	FVector opponentLocation = Opponent->GetActorLocation();
		//
		//	SetActorLocation(FVector(thisLocation.X, thisLocation.Y, opponentLocation.Z));
		//	
		//}
		//else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::W2_E)
		//{
		//	SkillDamage();		
		//	ExecSkillDamageData(1);
		//
		//	IsLaunchBySkill = false;
		//	IsAttackBySkill = false;
		//}
		//else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::W3_S)
		//{
		//	SkillDamage();
		//	ExecSkillDamageData(0);
		//}
		//else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::W3_E)
		//{
		//	SkillDamage();
		//	ExecSkillDamageData(1);
		//
		//	IsAttackBySkill = false;
		//}
		//else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::WC_S)
		//{
		//	//IsAttackBySkill = true;
		//
		//	SkillDamage();
		//	ExecSkillDamageData(1);
		//}
		//else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::WC_E)
		//{
		//	IsAttackBySkill = false;
		//}
		//else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::S1_S)
		//{
		//	SkillDamage();
		//	ExecSkillDamageData(0);
		//}
		//else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::S1_E)
		//{
		//	SkillDamage();
		//	ExecSkillDamageData(1);
		//
		//	IsAttackBySkill = false;
		//}
		//else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::S3_S)
		//{
		//	SkillDamage();
		//	ExecSkillDamageData(0);
		//}
		//else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::S3_E)
		//{
		//	SkillDamage();
		//	ExecSkillDamageData(1);
		//
		//	IsAttackBySkill = false;
		//}
		//else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::SF1_S)
		//{
		//	IsNowAttackBySF1 = true;
		//
		//	ActivateDamageIceEffect();	
		//	SkillDamage();
		//
		//	if (SpellFistDamageDatas[0].Montage)
		//		SpellFistDamageDatas[0].PlayMontage(this);
		//
		//	if (SpellFistDamageDatas[0].Effect)
		//		SpellFistDamageDatas[0].PlayEffect(GetWorld(), this);
		//	// MEMO: Ư�� DamageData ����
		//	//ExecSkillDamageData(3);
		//}
		//else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::SF1_E)
		//{
		//	DeactivateDamageIceEffect();	
		//	SkillDamage();
		//	// MEMO: Ư�� DamageData ����
		//	//ExecSkillDamageData(4);
		//	if (SpellFistDamageDatas[1].Montage)
		//		SpellFistDamageDatas[1].PlayMontage(this);
		//
		//	if (SpellFistDamageDatas[1].Effect)
		//		SpellFistDamageDatas[1].PlayEffect(GetWorld(), this);
		//
		//	IsNowAttackBySF1 = false;
		//	IsAttackBySkill = false;
		//}
		//else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::SF2_S)
		//{
		//	IsLaunchBySkill = true;
		//
		//	SkillDamage();
		//	//ExecSkillDamageData(0);
		//
		//	if (SpellFistDamageDatas[2].Montage)
		//		SpellFistDamageDatas[2].PlayMontage(this);
		//
		//	if (SpellFistDamageDatas[2].Effect)
		//		SpellFistDamageDatas[2].PlayEffect(GetWorld(), this);
		//
		//	FVector thisLocation = GetActorLocation();
		//	FVector opponentLocation = Opponent->GetActorLocation();
		//
		//	SetActorLocation(FVector(thisLocation.X, thisLocation.Y, opponentLocation.Z));
		//}
		//else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::SF2_E)
		//{
		//	SkillDamage();
		//	//ExecSkillDamageData(1);
		//
		//	if (SpellFistDamageDatas[3].Montage)
		//		SpellFistDamageDatas[3].PlayMontage(this);
		//
		//	if (SpellFistDamageDatas[3].Effect)
		//		SpellFistDamageDatas[3].PlayEffect(GetWorld(), this);
		//
		//	IsLaunchBySkill = false;
		//	IsAttackBySkill = false;
		//}
		//else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::SF3_S)
		//{
		//}
		//else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::SF3_E)
		//{
		//}
		if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::NormalHit)
		{
			DamageBySkillAndActivateEffect();
			SkillDamageData(0);
		}
		else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::BoundUpHit)
		{
			
			DamageBySkillAndActivateEffect();
			SkillDamageData(1);

			CheckTrue(IsBoundUp);
			
			IsBoundUp = true;
			
			GetWorldTimerManager().SetTimer(BoundUpTimer, this, &ACEnemy::TakeDamage_OpponentUsingSkill, 0.1f, true);
		}
		else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::AirborneHit)
		{
			IsLaunchBySkill = true;

			DamageBySkillAndActivateEffect();
			SkillDamageData(2);

			if (Opponent)
			{
				FVector targetLoc = Opponent->GetActorLocation();

				SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, targetLoc.Z));
			}
		}
		else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::StopHit)
		{
			IsSkillStopHit = true;
			
			ActivateDamageIceEffect();
			DamageBySkillAndActivateEffect();
			SkillDamageData(3);
		}
		else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::FinalHit)
		{
			DamageBySkillAndActivateEffect();
			SkillDamageData(4);
			
			IsAttackBySkill = false;

			if (IsLaunchBySkill)
				IsLaunchBySkill = false;

			if (IsSkillStopHit)
			{
				DeactivateDamageIceEffect();
				CustomTimeDilation = 1.0f;
				
				IsSkillStopHit = false;
			}

			if (IsBoundUp)
			{
				GetWorldTimerManager().ClearTimer(BoundUpTimer);

				IsBoundUp = false;
			}
		}

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

void ACEnemy::TakeDamage_OpponentSpellFistAttack()
{
	CheckTrue(CurrentStateType == EEnemyStateType::Dead);

	ICInterface_PlayerState* playerInterface = Cast<ICInterface_PlayerState>(Opponent);

	if (playerInterface && IsAttackBySpellFist)
	{
		bActivateRotateToOpponent = false;
		
		if (playerInterface->GetCurrentPlayerSpellFistType() == EPlayerSpellFistType::SF_S)
		{
			if (SpellFistDamageDatas[0].Effect)
				SpellFistDamageDatas[0].PlayEffect(GetWorld(), this);
			
			if (SpellFistDamageDatas[0].Montage)
				SpellFistDamageDatas[0].PlayMontage(this);
		}
		else if (playerInterface->GetCurrentPlayerSpellFistType() == EPlayerSpellFistType::SF_E)
		{
			if (SpellFistDamageDatas[1].Effect)
				SpellFistDamageDatas[1].PlayEffect(GetWorld(), this);

			if (SpellFistDamageDatas[1].Montage)
				SpellFistDamageDatas[1].PlayMontage(this);
		}

		// ������ ����
		float applyDamage = UKismetMathLibrary::RandomIntegerInRange(250, 300);

		Hp -= applyDamage;

		// �ǰ� ȸ�� 
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

		// �ǰ� ȿ�� 
		{
			PlayDamageSound();
			
			// MEMO: �� �߰��� Damage Effect�� 5��° �迭 ���ҿ� �־� ��
			//if (DamageDatas[5].Effect)
				//DamageDatas[5].PlayEffect(GetWorld(), this);
		}

		// Widget ���� ȿ��
		{
			ShowHitNumber(applyDamage, this->GetActorLocation());
			ShowHealthBar();

			Damaged.EventInstigator = Opponent->GetController();
			ShakeCamera(Damaged);
		}

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

void ACEnemy::BoundUpBySkill()
{
	//IsBoundUp = true;
	
	//LaunchCharacter(FVector(0.0f, 0.0f, 1000.0f), true, true);
}

//void ACEnemy::SF2_E_TakeDamage()
//{
//	if (SpellFistDamageDatas[4].Montage)
//		SpellFistDamageDatas[4].PlayMontage(this);
//
//	if (SpellFistDamageDatas[4].Effect)
//		SpellFistDamageDatas[4].PlayEffect(GetWorld(), this);
//}
//
//void ACEnemy::TakeDamage_OpponentSkillWeapon()
//{
//	if (IsAttackBySkillWeapon)
//	{
//		ICInterface_PlayerState* playerInterface = Cast<ICInterface_PlayerState>(Opponent);
//
//		if (playerInterface)
//		{
//			// �ǰ� ȸ�� 
//			{
//			FVector start = GetActorLocation();
//			FVector target = Opponent->GetActorLocation();
//
//			FVector direction = target - start;
//			direction.Normalize();
//
//			FTransform transform;
//			transform.SetLocation(GetActorLocation());
//
//			SetActorRotation(FRotator(GetActorRotation().Pitch, direction.Rotation().Yaw, GetActorRotation().Roll)/*UKismetMathLibrary::FindLookAtRotation(start, target)*/);
//			LaunchCharacter(-direction * 1.0f, true, false);
//		}
//			// �ǰ� ȿ�� 
//			{
//			PlayDamageSound();
//
//			ShowHealthBar();
//			
//			Damaged.EventInstigator = Opponent->GetController();
//			ShakeCamera(Damaged);
//			// MEMO: �� �߰��� Damage Effect�� 5��° �迭 ���ҿ� �־� ��
//			//if (DamageDatas[5].Effect)
//				//DamageDatas[5].PlayEffect(GetWorld(), this);
//		}
//
//			// Hp 0 ����
//			if (Hp <= 0.0f)
//		{
//			SetCurrentEnemyStateType(EEnemyStateType::Dead);
//
//			IsDeadBySkill = true;
//
//			for (int i = 0; i < Weapons.Num(); i++)
//			{
//				if (Weapons[i])
//					Weapons[i]->DestroyWeapon();
//			}
//		}
//
//			if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::SF1_S)
//			{
//				SkillDamage();
//				ExecSkillDamageData(0);
//
//				IsAttackBySkillWeapon = false;
//			}
//			else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::SF2_E)
//			{
//				FTimerHandle timer;
//				
//				GetWorldTimerManager().SetTimer(timer, this, &ACEnemy::SF2_E_TakeDamage, true);
//			}
//		}
//	}
//}

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
	//// MEMO: Enemy�� ��Ÿ�ְ� ��� �� ���������� ����� ��� ��尡 �������� ��������
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

	//// MEMO: �ӽ� ���� �ʿ�
	//if (!bActivateRotateToOpponent)
	//	bActivateRotateToOpponent = true;

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
