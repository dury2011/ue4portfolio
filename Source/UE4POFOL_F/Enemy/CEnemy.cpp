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
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Component/CCharacterComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetTextLibrary.h"
#include "Weapon/CWeapon.h"
#include "GameplayTagContainer.h"
#include "Components/SphereComponent.h"
#include "Player/CCannon.h"
#include "Enemy/CEnemy_Meele.h"

int32 ACEnemy::SpawnCount = 0;

// public: //////////////////////////////////////////////////////////////////////
ACEnemy::ACEnemy()
	:HealthBarDisplayTime(2.f)
{
	RootComponent = GetCapsuleComponent();
	
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	CurrentStateType = EEnemyStateType::IdleOrJustMoving;
	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("Health Bar Widget");
	HealthBarWidgetComponent->SetupAttachment(GetMesh());
	HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidgetComponent->SetDrawSize(FVector2D(125.0f, 15.0f));

	GetCharacterMovement()->GravityScale = 5.0f;

	//BoxComponentAttack = CreateDefaultSubobject<UBoxComponent>("Box Component Attack");

	//TeamId = FGenericTeamId(ID);
	//SphereComponentDetectOpponent = CreateDefaultSubobject<USphereComponent>("Sphere Component Detect Opponent");
	
	//if (SphereComponentDetectOpponent)
		//SphereComponentDetectOpponent->SetupAttachment(RootComponent);
}

void ACEnemy::Destroyed()
{

}

void ACEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (!Opponent)
	//{
	//	TArray<AActor*> outActorArr;
	//
	//	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("PlayerFriend"), outActorArr);
	//
	//	int selectElement = UKismetMathLibrary::RandomIntegerInRange(0, outActorArr.Num() - 1);
	//
	//	int32 targettedCount = Cast<ACEnemy>(outActorArr[selectElement])->GetTargettedActorCount();
	//
	//	if (targettedCount <= 3)
	//	{
	//		if (outActorArr[selectElement])
	//		{
	//			Opponent = dynamic_cast<ACEnemy*>(outActorArr[selectElement]);
	//		}
	//	}
	//	else if (targettedCount > 3)
	//	{
	//		TArray<AActor*>outPlayerArr;
	//
	//		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Player"), outActorArr);
	//
	//		for (int i = 0; i < outPlayerArr.Num(); i++)
	//		{
	//			if (outPlayerArr[i])
	//				Opponent = dynamic_cast<ACPlayer*>(outPlayerArr[i]);
	//		}
	//	}
	//}

	// Hp�� 0.0 ���� �۰ų� ���� ���
	if (Hp <= 0.0f && IsOnce_HpZeroed)
	{
		IsOnce_HpZeroed = false;
		
		if (OnEnemyDiedStopAI.IsBound())
		{
			GetComponents<UCapsuleComponent>(CapsuleCollisions);

			for (UShapeComponent* collision : CapsuleCollisions)
			{
				collision->SetCollisionProfileName(FName("Died"));
			}
			
			OnEnemyDiedStopAI.Broadcast();

			SetCurrentEnemyStateType(EEnemyStateType::Dead);

			for (int i = 0; i < Weapons.Num(); i++)
			{
				if (Weapons[i])
					Weapons[i]->DestroyWeapon();
			}

			if (DeadDatas[0].Montage)
			{
				DeadDatas[0].PlayMontage(this);

				ActivateDeadEffect();
			}

			return;
		}
	}

	// ���(Player)�� �����ϰ� ��븦 ���Ͽ� Yawȸ���� ������ ��츸 �ڵ� ����
	if (Opponent && bActivateRotateToOpponent)
	{
		CheckTrue(CurrentStateType == EEnemyStateType::Dead);
		
		FVector directionToOpponent = Opponent->GetActorLocation() - GetActorLocation();
		directionToOpponent.Z = 0.0f;
		FRotator targetRotation = FRotationMatrix::MakeFromX(directionToOpponent).Rotator();

		SetActorRotation(FMath::RInterpTo(GetActorRotation(), targetRotation, DeltaTime, RotationSpeed));
	}

	// ��밡 ������ ��츸 �ڵ� ����
	if (Opponent)
	{
		DistanceToOpponent = GetDistanceTo(this);
	}

	// Enemy�� Strafing�� ������ ��츸 �ڵ� ����
	if (CanStrafing)
	{
		AddMovementInput(StrafeDirection);
	}

	// Player���� ������ �޴� ��(���� �� �ִ� ����)�̰� ��ų�� ���� �������� ����� ������ ��츸 �ڵ� ����
	if (IsAttackByPlayer && IsLaunchBySkill && !IsBoss)
	{
		SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, Player->GetActorLocation().Z));
	}

	// Boss�� scale�� ũ�Ƿ� ��� ���� ���� ���� ������ ����� �ȵǸ� Player ���� ������ ����Ƿ� ������ true�� ����
	if (!IsAttackByPlayer && IsLaunchBySkill && IsBoss)
	{
		IsAttackByPlayer = true;
	}

	/* MEMO: Player�� Enemy�ǰ� �ݸ��� �����ȿ� ������ �� Ŭ������ IsAttackBySkill�� true�� �ǰ�
	 * ��Ƽ���̿����� ��������Ʈ�� ���ε� �� �Լ��� TakeDamage_~�Լ��� ȣ��ȴ�.
	 * ������ Player�� ��ų ��� ���� �ݸ��� ���� ������ Enemy�� ������ IsAttackBySkill�� false�� �Ǿ� 
	 * �ǰ� ���� ����� �ǰ� ��Ƽ������ ���� ���� ���޹��� ���� ��� ���� �� ä�� ���ְ� �Ǿ� �Ʒ� �ذ�å�� �ۼ��� 
	 */
	// Player Skill�� StopHit������ ��츸 �ڵ� ����
	if (IsSkillStopHit && IsAttackByPlayer && !IsBoss)
	{
		ICInterface_PlayerState* playerInterface = Cast<ICInterface_PlayerState>(Player);

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

	/* Player Skill Weapon�� Tag�� "SFWeapon"���� ������ ��
	* Player Skill Weapon���� Follow�� SphereCollision�� Tag�� "Sphere_Following"���� ������ ��
	*/
	// Player Skill�� BoundUp������ ��츸 �ڵ� ���� (Spell 3�� ° ��ų, Spell Fist 2�� ° ��ų�� �ش�)
	// Player�� ���� ���� Collision ������ ������ �� ������ ����Ǹ� �ȵ�
	if (IsBoundUpBySkill /*&& IsAttackByPlayer*/ && !IsBoss)
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
}

float ACEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	/* MEMO: Player Projectile ���� �̿��� ���� �������� 
	 * TakeDamage�Լ��� �ƴ� �ٸ� �Լ��� ȣ��Ǿ� ����� 
	 * �� �Լ��� Projectile�� Hit, BeginOverlap�� ��츸 ��� */

	// �Լ� ��ü ���� �� ���� Ȯ��
	{
		if (CurrentStateType == EEnemyStateType::Dead)
			return 0.0f;
	}
				
	// Damaged����ü �� �Ҵ� �� -Hp 
	{
		Damaged.DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		Damaged.EventInstigator = EventInstigator;
		Damaged.DamageCauser = DamageCauser;
		//Damaged.DamageEvent = /* (FActionDamageEvent*)&DamageEvent; */ // EXPLAIN: ���⼭ TakeDamage�� Character�� HitData�� �Ҵ��
		Hp -= Damaged.DamageAmount;
	}
	// Enemy Projectile �ǰݽ� ȸ���� �˹� ����

	//if(EventInstigator->GetOwner()->ActorHasTag("Player"))
	//{
	{
		FVector start = GetActorLocation();
		FVector target = Damaged.EventInstigator->GetPawn()->GetActorLocation();

		FVector direction = target - start;
		direction.Normalize();

		FTransform transform;
		transform.SetLocation(GetActorLocation());

		SetActorRotation(FRotator(GetActorRotation().Pitch, direction.Rotation().Yaw, GetActorRotation().Roll)/*UKismetMathLibrary::FindLookAtRotation(start, target)*/);
		
		if(ActivateDamageLaunch && !GetCharacterMovement()->IsFalling())
			LaunchCharacter(direction * - DamageLaunchDistance, true, true);
	}
	//}
	
	// DeadCheck
	{
		CheckDead();
	}

	// Projectile�� Begin Overlap �Ǿ��� ���� �۵��ϴ� ��
	
	//{
	//	ICInterface_PlayerState* playerInterface = Cast<ICInterface_PlayerState>(Opponent);

	//	if (playerInterface && (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::NormalHit))
	//		GetNormalDamageData(0);
	//	else if (playerInterface && (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::BoundUpHit))
	//		GetNormalDamageData(1);
	//	else if (playerInterface && (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::FinalHit))
	//		GetNormalDamageData(2);
	//}

	// Widget �� ���� ȿ��
	{
		ShowHitNumber(DamageAmount, this->GetActorLocation());
		ShowHealthBar();
		ShakeCamera(Damaged);
	}

	// Damage Check
	if (Hp > 0.0f)
	{
		ActivateDamageEffect(true, Player->CharacterComponent->GetCurrentWeaponType());
	}
	
	return Damaged.DamageAmount;
}

void ACEnemy::OnAttack()
{
	CheckTrue(CurrentStateType == EEnemyStateType::Dead);

	//for (int i = 0; i < ActionDatas.Num(); i++)
	//	CheckFalse(ActionDatas[i].Montage);

	CheckTrue(CurrentStateType == EEnemyStateType::Attack);
	SetCurrentEnemyStateType(EEnemyStateType::Attack);
}

void ACEnemy::BeginStrafing()
{
	CanStrafing = true;

	//StrafeDirection = GetActorForwardVector() * -0.5f;
}

void ACEnemy::ChangeStrafing()
{	
	CheckTrue(CurrentStateType == EEnemyStateType::Attack);
	CheckTrue(CurrentStateType == EEnemyStateType::Dead);

	typedef EEnemyStrafingType s;

	int32 select = UKismetMathLibrary::RandomIntegerInRange(1, 3);
	
	switch ((EEnemyStrafingType) select)
	{
		case s::Front :
		{
			CurrentStrafingType = s::Front;
			
			StrafeDirection = GetActorForwardVector() * 1.0f;
	
			break;
		}
		case s::Back:
		{
			CurrentStrafingType = s::Back;
			
			StrafeDirection = GetActorForwardVector() * -1.0f;
	
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
	
			StrafeDirection = GetActorRightVector() * 1.0f;
			
			break;
		}
		default:
		{
			StrafeDirection = FVector::ZeroVector;
	
			break;
		}
	}
}

void ACEnemy::EndStrafing()
{
	CanStrafing = false;

	StrafeDirection = FVector::ZeroVector;
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

void ACEnemy::OnStateTypeChange(EEnemyStateType InCurrentStateType)
{
	PreviousStateType = CurrentStateType;
	CurrentStateType = InCurrentStateType;

	if (OnEnemyStateTypeChanged.IsBound())
		OnEnemyStateTypeChanged.Broadcast(PreviousStateType, CurrentStateType);
}

void ACEnemy::SetTarget(bool InBool, AActor* InActor)
{
	//if (!Opponent)
	//{
		Opponent = Cast<ACharacter>(InActor);

		//if (Opponent)
		//{
		//	Cast<ACPlayer>(Opponent)->OnPlayerSkillAttack.AddDynamic(this, &ACEnemy::TakeDamage_OpponentUsingSkill);
		//	Cast<ACPlayer>(Opponent)->OnPlayerNormalAttack.AddDynamic(this, &ACEnemy::TakeDamage_OpponentNormalAttack);
		//	Cast<ACPlayer>(Opponent)->OnSpawnPlayerFriend.AddDynamic(this, &ACEnemy::ChangeOpponentSpawn);
		//}
	//}
}

ACEnemy* ACEnemy::SpawnEnemy(AActor* InSpawner, TSubclassOf<ACEnemy> InSpawnEnemyClass)
{
	if (InSpawnEnemyClass)
	{
		FActorSpawnParameters params;
		params.Owner = InSpawner;

		return InSpawner->GetWorld()->SpawnActor<ACEnemy>
			(
				InSpawnEnemyClass,
				InSpawner->GetActorTransform(),
				params
			);
	}
	else return nullptr;
}

ACEnemy* ACEnemy::SpawnEnemy(AActor* InSpawner, TSubclassOf<ACEnemy> InSpawnEnemyClass, FName InSpawnSocketName)
{
	if (InSpawnEnemyClass)
	{
		FActorSpawnParameters params;
		params.Owner = InSpawner;

		ACharacter* character = Cast<ACharacter>(InSpawner);

		if (character)
		{
			return InSpawner->GetWorld()->SpawnActor<ACEnemy>
				(
					InSpawnEnemyClass,
					character->GetMesh()->GetSocketTransform(InSpawnSocketName),
					params
				);
		}
		else return nullptr;
	}
	else return nullptr;
}

ACEnemy* ACEnemy::SpawnEnemy(AActor* InSpawner, TSubclassOf<ACEnemy> InSpawnEnemyClass, FTransform InSpawnTransform)
{
	if (InSpawnEnemyClass)
	{
		FActorSpawnParameters params;
		params.Owner = InSpawner;

		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		return InSpawner->GetWorld()->SpawnActor<ACEnemy>
			(
				InSpawnEnemyClass,
				InSpawnTransform,
				params
			);
	}
	else return nullptr;
}

void ACEnemy::DestroyEnemy()
{
	if (IsBoss)
	{
		SpawnCount -= 1;

		if (SpawnCount <= 0)
			SpawnCount = 0;
	}

	if (OnEnemyDied.IsBound())
		OnEnemyDied.Broadcast();
	
	Destroy();
}

void ACEnemy::TakeDamage_OpponentNormalAttack()
{
	CheckFalse(IsOnce_HpZeroed);
	
	ICInterface_PlayerState* playerInterface = Cast<ICInterface_PlayerState>(Player);
	
	if (playerInterface && IsAttackByPlayer)
	{
		// ��� ��
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

		// �ǰ� ������ ���� �� �ǰ� ������ UI ����
		{
			float applyDamage = UKismetMathLibrary::RandomIntegerInRange(100, 250);
			Hp -= applyDamage;

			ShowHitNumber(applyDamage, this->GetActorLocation());
			
			Damaged.EventInstigator = Cast<AController>(Player);
			
			ShakeCamera(Damaged);
			ShowHealthBar();
		}
		
		// �ǰ� ȸ�� 
		{
			// �ǰ� �̿� ȸ�� ����
			bActivateRotateToOpponent = false;
			
			FVector start = GetActorLocation();
			FVector target = Player->GetActorLocation();

			FVector direction = target - start;
			direction.Normalize();

			FTransform transform;
			transform.SetLocation(GetActorLocation());

			SetActorRotation(FRotator(GetActorRotation().Pitch, direction.Rotation().Yaw, GetActorRotation().Roll)/*UKismetMathLibrary::FindLookAtRotation(start, target)*/);
			
			if(ActivateDamageLaunch)
				LaunchCharacter(-direction * DamageLaunchDistance, true, false);
		}

		// �ǰ� ���� ��Ÿ��, ����Ʈ ���
		if (Hp > 0.0f)
		{
			ActivateDamageEffect(false, Player->CharacterComponent->GetCurrentWeaponType());

			DamagedByOpponentNormal_SkillAndFx(1000.0f);
			
			SetCurrentEnemyStateType(EEnemyStateType::Damage);

			if (playerInterface->GetCurrentPlayerNormalAttackType() == EPlayerNormalAttackType::NormalAttack)
				GetNormalDamageData(0);
			else if (playerInterface->GetCurrentPlayerNormalAttackType() == EPlayerNormalAttackType::BoundUpAttack)
				GetNormalDamageData(1);
			else if (playerInterface->GetCurrentPlayerNormalAttackType() == EPlayerNormalAttackType::FinalAttack)
				GetNormalDamageData(2);
		}
	}
}

void ACEnemy::TakeDamage_OpponentUsingSkill()
{
	CheckFalse(IsOnce_HpZeroed);
	
	CheckTrue(CurrentStateType == EEnemyStateType::Dead);
	
	ICInterface_PlayerState* playerInterface = Cast<ICInterface_PlayerState>(Player);
	
	// Player�� ���� ���� Collision���� Enemy�� ���̸� BoundUp ���� ������ ���� �����Ƿ� !IsAttackByPlayer ���� ��츦 ����Ѵ�.
	if (playerInterface && !IsAttackByPlayer && playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::BoundUpHit)
	{
		DamagedByOpponentNormal_SkillAndFx();
		GetSkillDamageData(1);
	}

	//Player���� Skill�� ���ݴ��ϴ� ���̸�
	if (IsAttackByPlayer && playerInterface)
	{
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

		SetCurrentEnemyStateType(EEnemyStateType::Damage);

		if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::NormalHit)
		{
			DamagedByOpponentNormal_SkillAndFx();
			GetSkillDamageData(0);
		}
		else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::BoundUpHit)
		{
			ActivateDamageLaunch = false;
			
			DamagedByOpponentNormal_SkillAndFx();
			GetSkillDamageData(1);

			/* Hp-�� ��Ÿ�� ���, Fx ������ �Ǿ�� ������ 
			 * Ÿ�̸� ������ �ٽ� �ϸ� �ȵǹǷ�, bool ���� üũ 
			 */
			CheckTrue(IsBoundUpBySkill);
			
			IsBoundUpBySkill = true;
			
			GetWorldTimerManager().SetTimer(BoundUpTimer, this, &ACEnemy::TakeDamage_OpponentUsingSkill, 0.1f, true);
		}
		else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::AirborneHit)
		{
			IsLaunchBySkill = true;

			DamagedByOpponentNormal_SkillAndFx();
			GetSkillDamageData(2);

			if (Player && !IsBoss)
			{
				FVector targetLoc = Player->GetActorLocation();

				SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, targetLoc.Z));
			}
		}
		else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::StopHit)
		{
			IsSkillStopHit = true;
			
			//DamagedByOpponentNormal_SkillAndFx();
			ActivateDamageIceEffect();
			GetSkillDamageData(3);
		}
		else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::FinalHit)
		{
			DamagedByOpponentNormal_SkillAndFx();
			GetSkillDamageData(4);
			
			IsAttackByPlayer = false;

			if (IsLaunchBySkill)
			{
				IsLaunchBySkill = false;

				ActivateDamageLaunch = true;
			}

			if (IsSkillStopHit)
			{
				DeactivateDamageIceEffect();
				CustomTimeDilation = 1.0f;
				
				IsSkillStopHit = false;
			}

			if (IsBoundUpBySkill)
			{
				GetWorldTimerManager().ClearTimer(BoundUpTimer);

				IsBoundUpBySkill = false;
			}
		}
	}
}

void ACEnemy::TakeDamage_OpponentUsingSkillWeapon()
{
	CheckFalse(IsOnce_HpZeroed);
	
	if (IsAttackBySkillWeapon)
	{
		ICInterface_PlayerState* playerInterface = Cast<ICInterface_PlayerState>(Player);
		
		if (playerInterface)
		{
			// �ǰ� ������, �ǰ� UI, �ǰ� ����Ʈ ����
			{
				float applyDamage = UKismetMathLibrary::RandomIntegerInRange(100, 250);
				Hp -= applyDamage;

				ShowHitNumber(applyDamage, this->GetActorLocation());

				Damaged.EventInstigator = Cast<AController>(Player);

				ShakeCamera(Damaged);
				ShowHealthBar();
				ActivateDamageEffect(false, Player->CharacterComponent->GetCurrentWeaponType());
			}

			// �ǰ� ȸ�� ����
			{
				bActivateRotateToOpponent = false;

				FVector start = GetActorLocation();
				FVector target = Player->GetActorLocation();

				FVector direction = target - start;
				direction.Normalize();

				FTransform transform;
				transform.SetLocation(GetActorLocation());

				SetActorRotation(FRotator(GetActorRotation().Pitch, direction.Rotation().Yaw, GetActorRotation().Roll));
			}
		
			// �ǰ� ��Ÿ�� ��� 
			{
				SetCurrentEnemyStateType(EEnemyStateType::Damage);
				
				if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::NormalHit)
				{
					DamagedByOpponentNormal_SkillAndFx();
					GetSkillDamageData(0);
				}
				else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::BoundUpHit)
					GetWorldTimerManager().SetTimer(SkillWeaponTimer, this, &ACEnemy::DamagedByOpponentSkillWeaponAndFx, 0.25f, true);
				else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::FinalHit)
					GetNormalDamageData(2);
			}

			// ��� ��
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

void ACEnemy::BlockedByShield()
{
	// ��� ȸ�� 
	{
		FVector start = GetActorLocation();
		FVector target = Player->GetActorLocation();

		FVector direction = target - start;
		direction.Normalize();

		FTransform transform;
		transform.SetLocation(GetActorLocation());

		SetActorRotation(FRotator(GetActorRotation().Pitch, direction.Rotation().Yaw, GetActorRotation().Roll)/*UKismetMathLibrary::FindLookAtRotation(start, target)*/);

		if (ActivateDamageLaunch)
			LaunchCharacter(-direction * 300.0f, true, false);
	}

	if (BlockedDatas[0].Montage)
		BlockedDatas[0].PlayMontage(this);

	if (BlockedDatas[0].Effect)
		BlockedDatas[0].PlayEffect(GetWorld(), this);
}

// private: //////////////////////////////////////////////////////////////////////
//void ACEnemy::CheckDamage()
//{
//	if (Hp > 0.0f)
//		//ActivateDamageEffect();
//}

void ACEnemy::DamagedByOpponentNormal_SkillAndFx(float InLaunchSpeed)
{
	bActivateRotateToOpponent = false;
	
	// ������ ����
	float applyDamage = UKismetMathLibrary::RandomIntegerInRange(250, 300);
	
	Hp -= applyDamage;
	
	// �ǰ� ȸ�� 
	{
		FVector start = GetActorLocation();
		FVector target = Player->GetActorLocation();

		FVector direction = target - start;
		direction.Normalize();

		FTransform transform;
		transform.SetLocation(GetActorLocation());

		SetActorRotation(FRotator(GetActorRotation().Pitch, direction.Rotation().Yaw, GetActorRotation().Roll)/*UKismetMathLibrary::FindLookAtRotation(start, target)*/);
		
		if(ActivateDamageLaunch)
			LaunchCharacter(-direction * DamageLaunchDistance, true, false);
	}

	// �ǰ� ȿ�� 
	{
		ActivateDamageEffect(false, Player->CharacterComponent->GetCurrentWeaponType());
		ShowHitNumber(applyDamage, this->GetActorLocation());
		ShowHealthBar();
	}
}

void ACEnemy::DamagedByOpponentSkillWeaponAndFx()
{
	if (IsAttackBySkillWeapon)
	{
		// �ǰ� ������ ���� �� �ǰ� ������ UI ����
		{
			float applyDamage = UKismetMathLibrary::RandomIntegerInRange(100, 250);
			Hp -= applyDamage;

			ShowHitNumber(applyDamage, this->GetActorLocation());

			Damaged.EventInstigator = Cast<AController>(Player);

			ShakeCamera(Damaged);
			ShowHealthBar();
		}

		// �ǰ� ȸ�� 
		{
			// �ǰ� �̿� ȸ�� ����
			bActivateRotateToOpponent = false;

			FVector start = GetActorLocation();
			FVector target = Player->GetActorLocation();

			FVector direction = target - start;
			direction.Normalize();

			FTransform transform;
			transform.SetLocation(GetActorLocation());

			SetActorRotation(FRotator(GetActorRotation().Pitch, direction.Rotation().Yaw, GetActorRotation().Roll));
		}

		ActivateDamageEffect(false, Player->CharacterComponent->GetCurrentWeaponType());

		SkillWeaponAttackCount++;

		if (SkillWeaponAttackCount != 5 && SkillWeaponAttackCount < 6)
		{
			if (SkillDamageDatas[0].Montage)
				SkillDamageDatas[0].PlayMontage(this);

			if (SkillDamageDatas[0].Effect)
				SkillDamageDatas[0].PlayEffect(GetWorld(), this);

			return;
		}
		if (SkillWeaponAttackCount != 5 && SkillWeaponAttackCount >= 6)
		{
			SkillWeaponAttackCount = 0;

			GetWorldTimerManager().ClearTimer(SkillWeaponTimer);

			ACPlayer* player = Cast<ACPlayer>(Player);

			if (player)
				player->Notify_SetCurrentPlayerSkillType(EPlayerSkillType::Max);

			return;
		}
		if (SkillWeaponAttackCount == 5)
		{
			ACPlayer* player = Cast<ACPlayer>(Player);

			if (player)
			{
				player->Notify_SetCurrentPlayerSkillType(EPlayerSkillType::FinalHit);

				TakeDamage_OpponentUsingSkillWeapon();
			}

			return;
		}
	}
}

void ACEnemy::GetNormalDamageData(int32 InIndex)
{
	if (NormalDamageDatas[InIndex].Montage)
		NormalDamageDatas[InIndex].PlayMontage(this);

	if (NormalDamageDatas[InIndex].Effect)
		NormalDamageDatas[InIndex].PlayEffect(GetWorld(), this);
}

void ACEnemy::GetSkillDamageData(int32 InIndex)
{
	if (SkillDamageDatas[InIndex].Montage)
		SkillDamageDatas[InIndex].PlayMontage(this);

	if (SkillDamageDatas[InIndex].Effect)
		SkillDamageDatas[InIndex].PlayEffect(GetWorld(), this);
}

void ACEnemy::CheckDead()
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

/* �÷��̾�� Enemy_Friend�� Spawn�ϸ� 
* Enemy�� Opponent�� Player Ȥ�� EnemyFriend�� �����ϰ� �����Ѵ�. 
* �� ������ �ִµ� Opponent�� 3���� �̻󿡰� Ÿ���� �̹� �Ǿ��ִٸ�, Player�� Ÿ���� �����Ѵ�.
* Ÿ���� ���õǸ� */
void ACEnemy::ChangeOpponentSpawn()
{
	//if (!IsPlayerFriendly)
	//{
	//	//int selectSwitch = UKismetMathLibrary::RandomIntegerInRange(0, 1);
	//	//
	//	//// Opponent�� Player�� 
	//	//if (selectSwitch == 0)
	//	//{
	//	//	TArray<AActor*> outActorArr;

	//	//	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Player"), outActorArr);

	//	//	for (int i = 0; i < outActorArr.Num(); i++)
	//	//		if (outActorArr[i])
	//	//			Opponent = dynamic_cast<ACPlayer*>(outActorArr[i]);
	//	//}
	//	//// Opponent�� Enemy Friend��
	//	//else if (selectSwitch == 1)
	//	{
	//		FTimerHandle WaitHandle;
	//		float WaitTime = 1.5f; //�ð��� �����ϰ�

	//		GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
	//		{
	//			// ���⿡ �ڵ带 ġ�� �ȴ�.
	//			TArray<AActor*> outActorArr;

	//			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("PlayerFriend"), outActorArr);

	//			int selectElement = UKismetMathLibrary::RandomIntegerInRange(0, outActorArr.Num() - 1);

	//			int32 targettedCount = Cast<ACEnemy>(outActorArr[selectElement])->GetTargettedActorCount();

	//			if (targettedCount <= 3)
	//			{
	//				if (outActorArr[selectElement])
	//				{
	//					Opponent = dynamic_cast<ACEnemy*>(outActorArr[selectElement]);

	//					Cast<ACEnemy>(Opponent)->SetTargettedActorCount();
	//					Cast<ACEnemy>(Opponent)->SetOpponent(this);

	//				}
	//			}
	//			else if (targettedCount > 3)
	//			{
	//				//TArray<AActor*> outActorPlayerArr;

	//				//UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Player"), outActorPlayerArr);
	//			
	//				//for (int i = 0; i < outActorPlayerArr.Num(); i++)
	//				//	if (outActorPlayerArr[i])
	//				//		Opponent = dynamic_cast<ACPlayer*>(outActorPlayerArr[i]);
	//			}

	//		}), WaitTime, false);
	//	}
	//}
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

// protected: //////////////////////////////////////////////////////////////////////
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
		//if (IsPlayerFriendly && !InMission2)
		//{
		//	TArray<AActor*> outActorArr;
		//	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), outActorArr);

		//	int select = UKismetMathLibrary::RandomIntegerInRange(0, outActorArr.Num() - 1);

		//	Opponent = dynamic_cast<ACEnemy*>(outActorArr[select]);
		//}
		
		//if (!IsPlayerFriendly && !InMission2)
		//{
		//	TArray<AActor*> outActorArr;
		//	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPlayer::StaticClass(), outActorArr);

		//	for (int i = 0; i < outActorArr.Num(); i++)
		//		Opponent = dynamic_cast<ACPlayer*>(outActorArr[i]);
		//}


		if (InMission2)
		{
			TArray<AActor*> outActorArr;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACCannon::StaticClass(), outActorArr);

			for (int i = 0; i < outActorArr.Num(); i++)
				Opponent = dynamic_cast<ACharacter*>(outActorArr[i]);
		}
	}

	// ���� ��ų ���ݿ� ���� �ǰ� ���� �Լ� ���ε�
	//if (Opponent)
	//{
	//	if (!InMission2 && !IsPlayerFriendly)
	//	{
	//		Cast<ACPlayer>(Opponent)->OnPlayerSkillAttack.AddDynamic(this, &ACEnemy::TakeDamage_OpponentUsingSkill);
	//		Cast<ACPlayer>(Opponent)->OnPlayerNormalAttack.AddDynamic(this, &ACEnemy::TakeDamage_OpponentNormalAttack);
	//		Cast<ACPlayer>(Opponent)->OnSpawnPlayerFriend.AddDynamic(this, &ACEnemy::ChangeOpponentSpawn);
	//	}
	//}

	// HealthBar ���� Hidden ����
	{
		if (HealthBarWidgetComponent)
			HealthBarWidgetComponent->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
	}

	// ����ü ����
	{
		SetCurrentEnemyStateType(EEnemyStateType::IdleOrJustMoving);
	}

	// Player�� ã�Ƽ� ������ ���� �Լ� ���ε�
	{
		TArray<AActor*> outPlayers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPlayer::StaticClass(), outPlayers);

		for (int i = 0; i < outPlayers.Num(); i++)
		{
			Player = Cast<ACPlayer>(outPlayers[i]);
		}
		
		if (Player)
		{
			Cast<ACPlayer>(Player)->OnPlayerSkillAttack.AddDynamic(this, &ACEnemy::TakeDamage_OpponentUsingSkill);
			Cast<ACPlayer>(Player)->OnPlayerNormalAttack.AddDynamic(this, &ACEnemy::TakeDamage_OpponentNormalAttack);
			Cast<ACPlayer>(Player)->OnSpawnPlayerFriend.AddDynamic(this, &ACEnemy::ChangeOpponentSpawn);
		}
	}


	//if (IsTest) 
	//{
	//	if (Player)
	//	{
	//		Opponent = Player;
	//		
	//	}
	//}
	//// Ÿ�̸� ����
	//{
	//	GetWorldTimerManager().SetTimer(StrafeTimer, this, &ACEnemy::ChangeStrafing, ChangeStrafingTypeInterval, true);
	//}
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

void ACEnemy::StoreHitNumber(UUserWidget* InHitNumber, FVector InLocation)
{
	HitNumbers.Add(InHitNumber, InLocation);

	FTimerHandle HitNumberTimer;
	FTimerDelegate HitNumberDelegate;
	HitNumberDelegate.BindUFunction(this, FName("DestroyHitNumber"), InHitNumber);
	GetWorld()->GetTimerManager().SetTimer(HitNumberTimer, HitNumberDelegate, HitNumberDestroyTime, false);
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

void ACEnemy::DestroyHitNumber(UUserWidget* InHitNumber)
{
	HitNumbers.Remove(InHitNumber);
	InHitNumber->RemoveFromParent(); // ����Ʈ���� ������ ������ ���̴�.
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
	if (!bActivateRotateToOpponent)
		bActivateRotateToOpponent = true;

	if (IsDeadBySkill)
	{
		ActivateDeadEffect();

		IsDeadBySkill = false;
	}

	if (CurrentStateType == EEnemyStateType::Damage)
		SetCurrentEnemyStateType(EEnemyStateType::IdleOrJustMoving);

	//// MEMO: �ӽ� ���� �ʿ�
	//if (!bActivateRotateToOpponent)
	//	bActivateRotateToOpponent = true;

	if (!Interrupted)
	{
		if (CurrentStateType == EEnemyStateType::Attack)
		{
			if (OnEnemyAttackEnded.IsBound())
				OnEnemyAttackEnded.Broadcast();

			SetCurrentEnemyStateType(EEnemyStateType::IdleOrJustMoving);

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

void ACEnemy::SetCurrentEnemyStateType(EEnemyStateType InType)
{
	PreviousStateType = CurrentStateType;
	CurrentStateType = InType;

	if (OnEnemyStateTypeChanged.IsBound())
		OnEnemyStateTypeChanged.Broadcast(PreviousStateType, CurrentStateType);
}

void ACEnemy::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void ACEnemy::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ACEnemy::OnBeginOverlapSphereComponentDetectOpponent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CheckNull(OtherActor);
	
	//if (IsPlayerFriendly)
	//{
		//if (OtherActor->ActorHasTag("Enemy"));
			//TArray<AActor*> InActorArr
	//}


	//if (OtherActor->ActorHasTag("Player"))
	//{
	//	ACPlayer* player = Cast<ACPlayer>(OtherActor);
	//
	//	if (player)
	//		player->SetIsAttackByEnemy(true, this);
	//}
	//else if (OtherActor->ActorHasTag("PlayerFriend"))
	//{
	//	ACEnemy* playerFriend = Cast<ACEnemy>(OtherActor);
	//
	//	if (playerFriend)
	//		playerFriend->SetIsEnemyFriendAttackByEnemy(true);
	//}
	//else if (OtherActor->ActorHasTag("Enemy"))
	//{
	//	ACEnemy* enemy = Cast<ACEnemy>(OtherActor);
	//
	//	if (enemy)
	//		enemy->SetIsEnemyAttackByEnemyFriend(true);
	//}
}

void ACEnemy::OnEndOverlapSphereComponentDetectOpponent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CheckNull(OtherActor);

	//if (OtherActor->ActorHasTag("Player"))
	//{
	//	ACPlayer* player = Cast<ACPlayer>(OtherActor);
	//
	//	player->SetIsAttackByEnemy(false, this);
	//}
	//else if (OtherActor->ActorHasTag("PlayerFriend"))
	//{
	//	ACEnemy* playerFriend = Cast<ACEnemy>(OtherActor);
	//
	//	playerFriend->SetIsEnemyFriendAttackByEnemy(false);
	//}
	//else if (OtherActor->ActorHasTag("Enemy"))
	//{
	//	ACEnemy* enemy = Cast<ACEnemy>(OtherActor);
	//
	//	if (enemy)
	//		enemy->SetIsEnemyAttackByEnemyFriend(false);
	//}
}

void ACEnemy::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}