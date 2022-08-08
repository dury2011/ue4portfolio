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

int32 ACEnemy::SpawnCount = 0;

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
		if (!InMission2)
		{
			TArray<AActor*> outActorArr;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPlayer::StaticClass(), outActorArr);

			for (int i = 0; i < outActorArr.Num(); i++)
				Opponent = dynamic_cast<ACPlayer*>(outActorArr[i]);
		}
		else if (InMission2)
		{
			TArray<AActor*> outActorArr;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACCannon::StaticClass(), outActorArr);

			for (int i = 0; i < outActorArr.Num(); i++)
				Opponent = dynamic_cast<ACharacter*>(outActorArr[i]);
		}
	}

	// 상대방 스킬 공격에 대한 피격 관련 함수 바인딩
	if (Opponent)
	{
		if (!InMission2)
		{
			Cast<ACPlayer>(Opponent)->OnPlayerSkillAttack.AddDynamic(this, &ACEnemy::TakeDamage_OpponentUsingSkill);
			Cast<ACPlayer>(Opponent)->OnPlayerNormalAttack.AddDynamic(this, &ACEnemy::TakeDamage_OpponentNormalAttack);
		}
	}

	// HealthBar 위젯 Hidden 설정
	{
		if (HealthBarWidgetComponent)
			HealthBarWidgetComponent->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
	}

	// 열거체 설정
	{
		SetCurrentEnemyStateType(EEnemyStateType::IdleOrJustMoving);
	}

	//// 타이머 설정
	//{
	//	GetWorldTimerManager().SetTimer(StrafeTimer, this, &ACEnemy::ChangeStrafing, ChangeStrafingTypeInterval, true);
	//}
}

void ACEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Hp가 0.0 보다 작거나 같을 경우
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

	// 상대(Player)가 존재하고 상대를 향하여 Yaw회전이 가능한 경우만 코드 실행
	if (Opponent && bActivateRotateToOpponent)
	{
		CheckTrue(CurrentStateType == EEnemyStateType::Dead);
		
		FVector directionToOpponent = Opponent->GetActorLocation() - GetActorLocation();
		directionToOpponent.Z = 0.0f;
		FRotator targetRotation = FRotationMatrix::MakeFromX(directionToOpponent).Rotator();

		SetActorRotation(FMath::RInterpTo(GetActorRotation(), targetRotation, DeltaTime, RotationSpeed));
	}

	// 상대가 존재할 경우만 코드 실행
	if (Opponent)
	{
		DistanceToOpponent = GetDistanceTo(this);
	}

	// Enemy가 Strafing이 가능한 경우만 코드 실행
	if (CanStrafing)
	{
		AddMovementInput(StrafeDirection);
	}

	// Player에게 공격을 받는 중(받을 수 있는 상태)이고 스킬에 의해 공중으로 띄워진 상태인 경우만 코드 실행
	if (IsAttackByPlayer && IsLaunchBySkill && !IsBoss)
	{
		SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, Opponent->GetActorLocation().Z));
	}

	// Boss는 scale이 크므로 에어본 공격 적용 안함 하지만 에어본이 안되면 Player 공격 범위에 벗어나므로 강제로 true로 설정
	if (!IsAttackByPlayer && IsLaunchBySkill && IsBoss)
	{
		IsAttackByPlayer = true;
	}

	/* MEMO: Player의 Enemy피격 콜리전 범위안에 들어오면 이 클래스의 IsAttackBySkill이 true가 되고
	 * 노티파이에의해 델리게이트에 바인드 된 함수인 TakeDamage_~함수가 호출된다.
	 * 하지만 Player가 스킬 사용 도중 콜리전 범위 밖으로 Enemy가 나가면 IsAttackBySkill이 false가 되어 
	 * 피격 도중 적용된 피격 머티리얼이 공격 끝을 전달받지 못해 계속 적용 된 채로 남있게 되어 아래 해결책을 작성함 
	 */
	// Player Skill이 StopHit공격일 경우만 코드 실행
	if (IsSkillStopHit && IsAttackByPlayer && !IsBoss)
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

	/* Player Skill Weapon의 Tag는 "SFWeapon"으로 지정할 것
	* Player Skill Weapon에서 Follow할 SphereCollision의 Tag는 "Sphere_Following"으로 지정할 것
	*/
	// Player Skill이 BoundUp공격일 경우만 코드 실행
	if (IsBoundUpBySkill && IsAttackByPlayer && !IsBoss)
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
	/* MEMO: Player Projectile 공격 이외의 공격 데미지는 
	 * TakeDamage함수가 아닌 다른 함수가 호출되어 적용됨 
	 * 이 함수는 Projectile에 Hit, BeginOverlap일 경우만 사용 */

	// 함수 몸체 실행 전 조건 확인
	{
		if (CurrentStateType == EEnemyStateType::Dead)
			return 0.0f;
	}
				
	// Damaged구조체 값 할당 및 -Hp 
	{
		Damaged.DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		Damaged.EventInstigator = EventInstigator;
		Damaged.DamageCauser = DamageCauser;
		//Damaged.DamageEvent = /* (FActionDamageEvent*)&DamageEvent; */ // EXPLAIN: 여기서 TakeDamage할 Character의 HitData가 할당됨
		Hp -= Damaged.DamageAmount;
	}
	// Enemy Projectile 피격시 회전과 넉백 설정
	{
		FVector start = GetActorLocation();
		FVector target = Damaged.EventInstigator->GetPawn()->GetActorLocation();

		FVector direction = target - start;
		direction.Normalize();

		FTransform transform;
		transform.SetLocation(GetActorLocation());

		SetActorRotation(FRotator(GetActorRotation().Pitch, direction.Rotation().Yaw, GetActorRotation().Roll)/*UKismetMathLibrary::FindLookAtRotation(start, target)*/);
		
		if(ActivateDamageLaunch)
			LaunchCharacter(-direction * 1000.0f, true, false);
	}
	
	// DeadCheck
	{
		CheckDead();
	}

	// Projectile에 Begin Overlap 되었을 때만 작동하는 곳
	{
		ICInterface_PlayerState* playerInterface = Cast<ICInterface_PlayerState>(Opponent);

		if (playerInterface && (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::NormalHit))
			GetNormalDamageData(0);
		else if (playerInterface && (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::BoundUpHit))
			GetNormalDamageData(1);
		else if (playerInterface && (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::FinalHit))
			GetNormalDamageData(2);
	}

	// Widget 및 각종 효과
	{
		ShowHitNumber(DamageAmount, this->GetActorLocation());
		ShowHealthBar();
		ShakeCamera(Damaged);
	}

	// Damage Check
	if (Hp > 0.0f)
	{
		CheckDamage();
	}
	
	return Damaged.DamageAmount;
}

void ACEnemy::TakeDamage_OpponentNormalAttack()
{
	CheckFalse(IsOnce_HpZeroed);
	
	ICInterface_PlayerState* playerInterface = Cast<ICInterface_PlayerState>(Opponent);
	
	if (playerInterface && IsAttackByPlayer)
	{
		// 사망 시
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

		// 피격 데미지 설정 및 피격 데미지 UI 설정
		{
			float applyDamage = UKismetMathLibrary::RandomIntegerInRange(100, 250);
			Hp -= applyDamage;

			ShowHitNumber(applyDamage, this->GetActorLocation());
			
			Damaged.EventInstigator = Cast<AController>(Opponent);
			
			ShakeCamera(Damaged);
			ShowHealthBar();
		}
		
		// 피격 회전 
		{
			// 피격 이외 회전 정지
			bActivateRotateToOpponent = false;
			
			FVector start = GetActorLocation();
			FVector target = Opponent->GetActorLocation();

			FVector direction = target - start;
			direction.Normalize();

			FTransform transform;
			transform.SetLocation(GetActorLocation());

			SetActorRotation(FRotator(GetActorRotation().Pitch, direction.Rotation().Yaw, GetActorRotation().Roll)/*UKismetMathLibrary::FindLookAtRotation(start, target)*/);
			
			if(ActivateDamageLaunch)
				LaunchCharacter(-direction * 1000.0f, true, false);
		}

		// 피격 에님 몽타주, 이펙트 재생
		if (Hp > 0.0f)
		{
			ActivateDamageEffect();

			DamagedByOpponentNormal_SkillAndFx(1000.0f);
			
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
	
	ICInterface_PlayerState* playerInterface = Cast<ICInterface_PlayerState>(Opponent);
	
	//Player에게 Skill로 공격당하는 중이면
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

		if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::NormalHit)
		{
			DamagedByOpponentNormal_SkillAndFx();
			GetSkillDamageData(0);
		}
		else if (playerInterface->GetCurrentPlayerSkillType() == EPlayerSkillType::BoundUpHit)
		{
			DamagedByOpponentNormal_SkillAndFx();
			GetSkillDamageData(1);

			/* Hp-와 몽타주 재생, Fx 실행은 되어야 하지만 
			 * 타이머 설정을 다시 하면 안되므로, bool 변수 체크 
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

			if (Opponent && !IsBoss)
			{
				FVector targetLoc = Opponent->GetActorLocation();

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
				IsLaunchBySkill = false;

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
		ICInterface_PlayerState* playerInterface = Cast<ICInterface_PlayerState>(Opponent);
		
		if (playerInterface)
		{
			// 피격 데미지, 피격 UI, 피격 이펙트 설정
			{
				float applyDamage = UKismetMathLibrary::RandomIntegerInRange(100, 250);
				Hp -= applyDamage;

				ShowHitNumber(applyDamage, this->GetActorLocation());

				Damaged.EventInstigator = Cast<AController>(Opponent);

				ShakeCamera(Damaged);
				ShowHealthBar();
				ActivateDamageEffect();
			}

			// 피격 회전 설정
			{
				bActivateRotateToOpponent = false;

				FVector start = GetActorLocation();
				FVector target = Opponent->GetActorLocation();

				FVector direction = target - start;
				direction.Normalize();

				FTransform transform;
				transform.SetLocation(GetActorLocation());

				SetActorRotation(FRotator(GetActorRotation().Pitch, direction.Rotation().Yaw, GetActorRotation().Roll));
			}
		
			// 피격 몽타주 재생 
			{
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

			// 사망 시
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
	// 블록 회전 
	{
		FVector start = GetActorLocation();
		FVector target = Opponent->GetActorLocation();

		FVector direction = target - start;
		direction.Normalize();

		FTransform transform;
		transform.SetLocation(GetActorLocation());

		SetActorRotation(FRotator(GetActorRotation().Pitch, direction.Rotation().Yaw, GetActorRotation().Roll)/*UKismetMathLibrary::FindLookAtRotation(start, target)*/);

		if (ActivateDamageLaunch)
			LaunchCharacter(-direction * 500.0f, true, false);
	}

	if (BlockedDatas[0].Montage)
		BlockedDatas[0].PlayMontage(this);

	if (BlockedDatas[0].Effect)
		BlockedDatas[0].PlayEffect(GetWorld(), this);
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

void ACEnemy::DamagedByOpponentNormal_SkillAndFx(float InLaunchSpeed)
{
	bActivateRotateToOpponent = false;
	
	// 데미지 적용
	float applyDamage = UKismetMathLibrary::RandomIntegerInRange(250, 300);
	
	Hp -= applyDamage;
	
	// 피격 회전 
	{
		FVector start = GetActorLocation();
		FVector target = Opponent->GetActorLocation();

		FVector direction = target - start;
		direction.Normalize();

		FTransform transform;
		transform.SetLocation(GetActorLocation());

		SetActorRotation(FRotator(GetActorRotation().Pitch, direction.Rotation().Yaw, GetActorRotation().Roll)/*UKismetMathLibrary::FindLookAtRotation(start, target)*/);
		
		if(ActivateDamageLaunch)
			LaunchCharacter(-direction * InLaunchSpeed, true, false);
	}

	// 피격 효과 
	{
		ActivateDamageEffect();

		// 더 추가한 Damage Effect를 5번째 배열 원소에 넣어 둠
		//if (SkillDamageDatas[5].Effect)
			//SkillDamageDatas[5].PlayEffect(GetWorld(), this);

		ShowHitNumber(applyDamage, this->GetActorLocation());
		ShowHealthBar();
	}
}

void ACEnemy::DamagedByOpponentSkillWeaponAndFx()
{
	if (IsAttackBySkillWeapon)
	{
		// 피격 데미지 설정 및 피격 데미지 UI 설정
		{
			float applyDamage = UKismetMathLibrary::RandomIntegerInRange(100, 250);
			Hp -= applyDamage;

			ShowHitNumber(applyDamage, this->GetActorLocation());

			Damaged.EventInstigator = Cast<AController>(Opponent);

			ShakeCamera(Damaged);
			ShowHealthBar();
		}

		// 피격 회전 
		{
			// 피격 이외 회전 정지
			bActivateRotateToOpponent = false;

			FVector start = GetActorLocation();
			FVector target = Opponent->GetActorLocation();

			FVector direction = target - start;
			direction.Normalize();

			FTransform transform;
			transform.SetLocation(GetActorLocation());

			SetActorRotation(FRotator(GetActorRotation().Pitch, direction.Rotation().Yaw, GetActorRotation().Roll));
		}

		ActivateDamageEffect();

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

			ACPlayer* player = Cast<ACPlayer>(Opponent);

			if (player)
				player->Notify_SetCurrentPlayerSkillType(EPlayerSkillType::Max);

			return;
		}
		if (SkillWeaponAttackCount == 5)
		{
			ACPlayer* player = Cast<ACPlayer>(Opponent);

			if (player)
			{
				player->Notify_SetCurrentPlayerSkillType(EPlayerSkillType::FinalHit);

				TakeDamage_OpponentUsingSkillWeapon();
			}

			return;
		}
	}
}

void ACEnemy::CheckDamage()
{
	if (Hp > 0.0f)
		ActivateDamageEffect();
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

void ACEnemy::OnAttack()
{
	CheckTrue(CurrentStateType == EEnemyStateType::Dead);

	//for (int i = 0; i < ActionDatas.Num(); i++)
	//	CheckFalse(ActionDatas[i].Montage);

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

	//// MEMO: 임시 수정 필요
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
	// MEMO: 몽타주 재생 도중 다른 몽타주가 재생될 경우 Interrupted == true
	else if (Interrupted)
	{
		GLog->Log("ACEnemy::OnMontageEnded Interrupted!");

		return;
	}
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

//void ACEnemy::SpawnEnemyEffectWeapon()
//{
//	for (int i = 0; i < EffectWeaponClass.Num(); i++)
//		CheckFalse(EffectWeaponClass[i]);
//
//	ACWeapon* effectWeapon = ACWeapon::SpawnWeapon(this, EffectWeaponClass[EffectWeaponIndex], GetMesh()->GetSocketLocation(EffectWeaponSpawnSocketName));
//	
//	if (effectWeapon)
//	{
//		effectWeapon->SetOwner(this);
//		effectWeapon->SetActorRotation(GetActorRotation());
//	}
//}
