/* MEMO: CPlayer.cpp �ڵ� ��ġ ���� �Ϸ� 
* .h ���Ͽ� ����� �Լ� ���� ������ �Լ� ���Ǹ� �ۼ��Ͽ���. */
#include "CPlayer.h"
#include "Global.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputComponent.h"
#include "Component/CIKComponent.h"
#include "Camera/CameraComponent.h"
#include "Animation/AnimInstance.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Weapon/CWeapon.h"
#include "Components/ArrowComponent.h"
#include "Enemy/CEnemy.h"
#include "Components/CapsuleComponent.h"
#include "Weapon/CDoAction.h"
#include "Component/CWidgetComponent.h"
#include "Component/CCharacterComponent.h"
#include "Weapon/CProjectile.h"
#include "Object/CPortalCrosshair.h"
#include "Object/CPortalDoor.h"
#include "Object/CCrosshair.h"
#include "Object/CCrosshair_SpellThrow.h"
#include "Components/SphereComponent.h"
#include "Interface/CInterface_Item.h"
#include "Enemy/CEnemy_Boss.h"
#include "GameFramework/ProjectileMovementComponent.h"

// public: //////////////////////////////////////////////////////////////////////
ACPlayer::ACPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = GetCapsuleComponent();
	BoxComponentSkill = CreateDefaultSubobject<UBoxComponent>("Box Collision For Skill");
	BoxComponentSkill->SetupAttachment(RootComponent);
	SphereComponentSpellFist = CreateDefaultSubobject<USphereComponent>("Sphere Collision For Spell Fist");
	SphereComponentSpellFist->SetupAttachment(RootComponent);

	{
		USkeletalMesh* mesh;
		CHelpers::GetAsset<USkeletalMesh>(&mesh, "SkeletalMesh'/Game/ParagonGideon/Characters/Heroes/Gideon/Meshes/Gideon.Gideon'");
		GetMesh()->SetSkeletalMesh(mesh);
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -100.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

		GetMesh()->SetupAttachment(GetCapsuleComponent());
	}
	
	{		
		SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
		SpringArmComponent->SetupAttachment(GetMesh());
		SpringArmComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		SpringArmComponent->TargetArmLength = 700.0f;
		SpringArmComponent->bDoCollisionTest = false;
		SpringArmComponent->bUsePawnControlRotation = true;
		SpringArmComponent->SocketOffset = FVector(0.0f, 0.0f, 0.0f);
		SpringArmComponent->bEnableCameraLag = true;
		SpringArmComponent->CameraLagSpeed = 7.5f;
		CHelpers::CreateComponent<UCameraComponent>(this, &CameraComponent, "Camera", SpringArmComponent);
		CHelpers::CreateComponent<USceneComponent>(this, &ArrowGroup, "ArrowGroup", GetCapsuleComponent());
	}
	
	{
		IKComponent = CreateDefaultSubobject<UCIKComponent>("CIKComponent");
		WidgetComponent = CreateDefaultSubobject<UCWidgetComponent>("CWidgetComponent");
		CharacterComponent = CreateDefaultSubobject<UCCharacterComponent>("CCharacterComponent");
		ParkourComponent = CreateDefaultSubobject<UCParkourComponent>("CParkourComponent");
	}

	{
		//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);//
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bUseControllerRotationYaw = true;
	}

	{
		const UEnum* ptr = FindObject<UEnum>(ANY_PACKAGE, L"EParkourArrowType", true);
	
		for (int32 i = 0; i < (int32)EParkourArrowType::Max; i++)
		{
			FName name = FName(ptr->GetNameStringByIndex(i));
			CHelpers::CreateComponent<UArrowComponent>(this, &Arrows[i], name, ArrowGroup);
	
			switch ((EParkourArrowType)i)
			{
			case EParkourArrowType::Center:
				Arrows[i]->ArrowColor = FColor::Red;
	
				break;
	
			case EParkourArrowType::Ceil:
				Arrows[i]->ArrowColor = FColor::Green;
				Arrows[i]->SetRelativeLocation(FVector(0, 0, 100));
	
				break;
	
			case EParkourArrowType::Floor:
				Arrows[i]->ArrowColor = FColor::Blue;
				Arrows[i]->SetRelativeLocation(FVector(0, 0, -80));
	
				break;
	
			case EParkourArrowType::Left:
				Arrows[i]->ArrowColor = FColor::Magenta;
				Arrows[i]->SetRelativeLocation(FVector(0, -30, 0));
	
				break;
	
			case EParkourArrowType::Right:
				Arrows[i]->ArrowColor = FColor::Magenta;
				Arrows[i]->SetRelativeLocation(FVector(0, 30, 0));
	
				break;
	
			case EParkourArrowType::Land:
				Arrows[i]->ArrowColor = FColor::Red;
				Arrows[i]->SetRelativeLocation(FVector(0, 0, -80));
				Arrows[i]->SetRelativeRotation(FRotator(-90, 0, 0));
	
				break;
			}
		}
	}

	ConstructorHelpers::FClassFinder<ACPortalCrosshair> assetCrosshair_Portal(TEXT("Blueprint'/Game/FORUE4POFOL/Player/Blueprint/BP_CPortalCrosshair.BP_CPortalCrosshair_C'"));

	if (assetCrosshair_Portal.Succeeded())
		PortalCrosshairClass = assetCrosshair_Portal.Class;

	ConstructorHelpers::FClassFinder<ACCrosshair> assetCrosshair_SpellMeteor(TEXT("Blueprint'/Game/FORUE4POFOL/Player/Blueprint/BP_CCrosshair_SpellMeteor.BP_CCrosshair_SpellMeteor_C'"));
	
	if (assetCrosshair_SpellMeteor.Succeeded())
		Crosshair_SpellMeteorClass = assetCrosshair_SpellMeteor.Class;

	//ConstructorHelpers::FClassFinder<ACProjectile> assetPortalProjectile(TEXT("Blueprint'/Game/FORUE4POFOL/Player/Blueprint/BP_CProjectile_Portal'"));
	//
	//if (assetPortalProjectile.Succeeded())
	//	PortalProjectileClass = assetPortalProjectile.Class;
	//
	//ConstructorHelpers::FClassFinder<ACPortalDoor> assetPortalDoorEntrance(TEXT("Blueprint'/Game/FORUE4POFOL/Player/Blueprint/BP_CPortalDoor_Enterance'"));
	//
	//if (assetPortalDoorEntrance.Succeeded())
	//	PortalDoorEntranceClass = assetPortalDoorEntrance.Class;
	//
	//ConstructorHelpers::FClassFinder<ACPortalDoor> assetPortalDoorExit(TEXT("Blueprint'/Game/FORUE4POFOL/Player/Blueprint/BP_CPortalDoor_Exit'"));
	//
	//if (assetPortalDoorExit.Succeeded())
		//PortalDoorExitClass = assetPortalDoorExit.Class;

	ConstructorHelpers::FClassFinder<ACWeapon> assetMeteorWeapon(TEXT("Blueprint'/Game/FORUE4POFOL/Player/Blueprint/BP_CWeapon_SpellMeteor.BP_CWeapon_SpellMeteor_C'"));

	if (assetMeteorWeapon.Succeeded())
		SpellMeteorClass = assetMeteorWeapon.Class;
	
	//TeamId = FGenericTeamId(ID);
}

void ACPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CanOnAction = OnActionChecker();

	Timeline.TickTimeline(DeltaTime);

	if (bParkouring)
		SetActorRotation(UKismetMathLibrary::RInterpTo(GetActorRotation(), ParkourTargetRot, GetWorld()->GetDeltaSeconds(), 10.0f));
	
	if (SpellSkill3Projectile && IsSpell3ProjectileSpawned && !IsSpell3ProjectileShoot)
	{
		SpellSkill3Projectile->SetActorLocation(GetMesh()->GetSocketLocation("Spawn_SpellSkill3_Projectile"));
		SpellSkill2ProjectileFinalSize = SpellSkill3Projectile->GetActorScale();
	}

	if (ParkourComponent->IsBeginClimbUpMode() || ParkourComponent->IsBeginClimbDownMode() || ParkourComponent->IsClimbingMode())
		IsParkouringClimbing = true;
	else
		IsParkouringClimbing = false;

	if (IsAirborne)
	{
		if (Boss)
			SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, Boss->GetActorLocation().Z));
	}

#ifdef LOG_PLAYER
	PlayerLog();
#endif 
}

void ACPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// TODO: Dash �����ϱ�
	PlayerInputComponent->BindAxis("MoveForward", this, &ACPlayer::OnMoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACPlayer::OnMoveRight);
	PlayerInputComponent->BindAxis("VerticalLook", this, &ACPlayer::OnVerticalLook);
	PlayerInputComponent->BindAxis("HorizontalLook", this, &ACPlayer::OnHorizontalLook);
	PlayerInputComponent->BindAxis("Zoom", this, &ACPlayer::OnZoom);
	PlayerInputComponent->BindAction("Dash", EInputEvent::IE_DoubleClick, this, &ACPlayer::OnDash);
	PlayerInputComponent->BindAction("Dash", EInputEvent::IE_Released, this, &ACPlayer::OffDash);
	PlayerInputComponent->BindAction("Run", EInputEvent::IE_Pressed, this, &ACPlayer::OnRun);
	PlayerInputComponent->BindAction("Run", EInputEvent::IE_Released, this, &ACPlayer::OffRun);
	PlayerInputComponent->BindAction("Run", EInputEvent::IE_DoubleClick, this, &ACPlayer::OnSpellTravel);
	PlayerInputComponent->BindAction("Onehand", EInputEvent::IE_Pressed, this, &ACPlayer::OnOnehand);
	PlayerInputComponent->BindAction("SpellFist", EInputEvent::IE_Pressed, this, &ACPlayer::OnSpellFist);
	PlayerInputComponent->BindAction("Spell", EInputEvent::IE_Pressed, this, &ACPlayer::OnSpell);
	PlayerInputComponent->BindAction("Skill1", EInputEvent::IE_Pressed, this, &ACPlayer::OnSkillOne);
	PlayerInputComponent->BindAction("Skill1", EInputEvent::IE_Released, this, &ACPlayer::OffSkillOne);
	PlayerInputComponent->BindAction("Skill2", EInputEvent::IE_Pressed, this, &ACPlayer::OnSkillTwo);
	PlayerInputComponent->BindAction("Skill3", EInputEvent::IE_Pressed, this, &ACPlayer::OnSkillThree);
	PlayerInputComponent->BindAction("Skill3", EInputEvent::IE_Released, this, &ACPlayer::OffSkillThree);
	PlayerInputComponent->BindAction("Action", EInputEvent::IE_Pressed, this, &ACPlayer::OnAction);
	PlayerInputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &ACPlayer::OnAim);
	PlayerInputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &ACPlayer::OffAim);
	PlayerInputComponent->BindAction("OnShield", EInputEvent::IE_Pressed, this, &ACPlayer::OnShield);
	PlayerInputComponent->BindAction("OnShield", EInputEvent::IE_Released, this, &ACPlayer::OffShield);
	PlayerInputComponent->BindAction("Parkour", EInputEvent::IE_Pressed, this, &ACPlayer::OnParkour);
	PlayerInputComponent->BindAction("Parkour", EInputEvent::IE_Released, this, &ACPlayer::OffParkour);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACPlayer::OnJump);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ACPlayer::OffJump);
	PlayerInputComponent->BindAction("Critical", EInputEvent::IE_Pressed, this, &ACPlayer::OnCritical);
	PlayerInputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &ACPlayer::OnSpawnFriend);
	PlayerInputComponent->BindAction("Debug", EInputEvent::IE_Pressed, this, &ACPlayer::Debug_OnControllerRotationYaw);
}

float ACPlayer::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Damaged.DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser); // damage float
	//Damaged.DamageEvent = (FActionDamageEvent*)&DamageEvent;  // Hit ���ϸ��̼� ��Ÿ��
	Damaged.EventInstigator = EventInstigator; // Player �̹Ƿ� Enemy controller
	Damaged.DamageCauser = DamageCauser; // weapon
	Damaged.DamageAmount = DamageAmount;
	
	CurrentCameraEffectType = ECameraEffectType::Damage;
	CharacterComponent->SetHp(-Damaged.DamageAmount);

	if (bParkouring)
		return 0.0f;

	CharacterComponent->SetCurrentStateType(EStateType::Damage);
	CurrentCameraEffectType = ECameraEffectType::Damage;
	//CharacterComponent->SetIsMontagePlaying(true);

	if (Damaged.DamageCauser->ActorHasTag("BossWeapon"))
	{
		GetDamageData(3);

		IsMontagePlaying = true;
		IsAirborne = false;
	}

	// Player ȸ�� & Knockback
	{
		if (Damaged.EventInstigator)
		{
			FVector target = Damaged.EventInstigator->GetPawn()->GetActorLocation();

			FVector start = GetActorLocation();
			FVector direction = target - start;
			direction.Normalize();

			FTransform transform;
			transform.SetLocation(GetActorLocation());

			SetActorRotation(FRotator(GetActorRotation().Pitch, direction.Rotation().Yaw, GetActorRotation().Roll)/*UKismetMathLibrary::FindLookAtRotation(start, target)*/);
			
			if (!GetCharacterMovement()->IsFalling())
			{
				LaunchCharacter(-direction * 1200.0f, true, false);
			}
		}
	}

	SpawnCameraEffect();
	ShakeCamera();
	
	if (CharacterComponent->GetCurrentHp() <= 0.0f)
	{
		GLog->Log("ACPlayer::TakeDamage() And Dead");

		return Damaged.DamageAmount;
	}
	
	GLog->Log("ACPlayer::TakeDamage()");

	return Damaged.DamageAmount;
}

void ACPlayer::TakeDamage_AttackByBoss(EBossAttackType InType)
{
	if (IsAttackByBoss)
	{
		if (bAttacking)
			bAttacking = false;
		if (IsMontagePlaying)
			IsMontagePlaying = false;
		
		float applyDamage = UKismetMathLibrary::RandomIntegerInRange(50, 70);
		CharacterComponent->SetHp(-applyDamage);
		CharacterComponent->SetCurrentStateType(EStateType::Damage);
		CurrentCameraEffectType = ECameraEffectType::Damage;
		
		ActivateDamageEffect();
		SpawnCameraEffect();
		ShakeCamera();
		
		CheckTrue(bParkouring);

		// Player �ǰݽ� ȸ���� �˹� ����
		{
			TArray<AActor*> outArray;
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy"), outArray);
			
			for (int i = 0; i < outArray.Num(); i++)
				if (outArray[i])
					Damaged.EventInstigator = outArray[i]->GetInstigatorController();

			if (Damaged.EventInstigator)
			{
				FVector target = Damaged.EventInstigator->GetPawn()->GetActorLocation();

				FVector start = GetActorLocation();
				FVector direction = target - start;
				direction.Normalize();

				FTransform transform;
				transform.SetLocation(GetActorLocation());

				SetActorRotation(FRotator(GetActorRotation().Pitch, direction.Rotation().Yaw, GetActorRotation().Roll)/*UKismetMathLibrary::FindLookAtRotation(start, target)*/);
				
				if (!(InType == EBossAttackType::AirborneAttack))
					LaunchCharacter(-direction * 10000.0, true, false);
			}
		}

		if (InType == EBossAttackType::NormalAttack)
		{
			//GetDamageData(0);

			//IsMontagePlaying = true;
		}
		else if (InType == EBossAttackType::BoundUpAttack)
		{
			GetDamageData(1);

			IsMontagePlaying = true;
		}
		else if (InType == EBossAttackType::AirborneAttack)
		{
			GetDamageData(4);

			IsAirborne = true;
		}
		else if (InType == EBossAttackType::GroggyAttack)
		{
			GetDamageData(2);

			IsMontagePlaying = true;
		}
		else if (InType == EBossAttackType::FinalAttack)
		{
			GetDamageData(3);

			bUseControllerRotationYaw = false;

			IsMontagePlaying = true;
			IsAirborne = false;
		}
	}
}

void ACPlayer::TakeDamage_AttackByEnemy()
{
	CurrentCameraEffectType = ECameraEffectType::Damage;
	CharacterComponent->SetHp(-Damaged.DamageAmount);

	if (bParkouring)
		return;
	
	// Player ȸ�� & Knockback
	{	
		FVector target = Enemy->GetActorLocation();

		FVector start = GetActorLocation();
		FVector direction = target - start;
		direction.Normalize();

		FTransform transform;
		transform.SetLocation(GetActorLocation());

		SetActorRotation(FRotator(GetActorRotation().Pitch, direction.Rotation().Yaw, GetActorRotation().Roll)/*UKismetMathLibrary::FindLookAtRotation(start, target)*/);
		LaunchCharacter(-direction * 500.0f, true, false);
	}

	SpawnCameraEffect();
	ShakeCamera();
}

void ACPlayer::SetIsAttackByEnemy(bool InBool, ACEnemy* InEnemy)
{
	IsAttackByEnemy = InBool;

	Enemy = InEnemy;

	InEnemy->OnEnemyAttack.AddDynamic(this, &ACPlayer::TakeDamage_AttackByEnemy);

}

void ACPlayer::EnableBind()
{
	TArray<AActor*> outActorArr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACEnemy_Boss::StaticClass(), outActorArr);

	for (int i = 0; i < outActorArr.Num(); i++)
		Boss = dynamic_cast<ACEnemy_Boss*>(outActorArr[i]);

	if (Boss)
		Boss->OnBossAttack.AddDynamic(this, &ACPlayer::TakeDamage_AttackByBoss);
}

void ACPlayer::Notify_BeginNextAction()
{
	if (bCanNextAction)
	{
		bCanNextAction = false;

		Index++;

		if (CharacterComponent->GetIsWeaponOnehandMode())
			CharacterComponent->GetActionDatasOnehand(Index).PlayMontage(this);
		else if (CharacterComponent->GetIsWeaponSpellFistMode())
			SpellFistDatas[Index].PlayMontage(this);

		CharacterComponent->SetSp(-500.0f);
	}
}

void ACPlayer::Notify_EndThisAction()
{
	Index = 0;
	bCanCombo = false;
	bCanNextAction = false;
	bAttacking = false;
}

void ACPlayer::Notify_OnNormalAttack()
{
	if (OnPlayerNormalAttack.IsBound())
		OnPlayerNormalAttack.Broadcast();
}

void ACPlayer::Notify_OnSkillAttack()
{
	if (OnPlayerSkillAttack.IsBound())
		OnPlayerSkillAttack.Broadcast();
}

void ACPlayer::Notify_OnSkillLaunch()
{
	if (OnPlayerSkillLaunch.IsBound())
		OnPlayerSkillLaunch.Broadcast();
}

void ACPlayer::Notify_OnSpellFistAttack()
{
	if (OnPlayerSpellFistAttack.IsBound())
		OnPlayerSpellFistAttack.Broadcast();
}

void ACPlayer::Notify_ParkourRotation()
{
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ACPlayer::Notify_OnParkourFinish()
{
	bParkouring = false;
	
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	
	CharacterComponent->SetIsMontagePlaying(false);
}

void ACPlayer::Notify_SetCurrentPlayerNormalAttackType(EPlayerNormalAttackType InType)
{
	CurrentPlayerNormalAttackType = InType;
}

void ACPlayer::Notify_SetCurrentPlayerSkillType(EPlayerSkillType InType)
{
	CurrentPlayerSkillType = InType;
}

void ACPlayer::Notify_SetCurrentPlayerSpellFistType(EPlayerSpellFistType InType)
{
	CurrentPlayerSpellFistType = InType;
}

void ACPlayer::SpawnProjectile()
{
	if (SpellSkill3ProjectileClass)
	{
		SpawnWarriorSkillOneEffect();

		SpellSkill3Projectile = ACProjectile::SpawnProjectile(this, SpellSkill3ProjectileClass, FName("Spawn_SpellSkill3_Projectile"));
		
		if (SpellSkill3Projectile)
		{
			IsSpell3ProjectileSpawned = true;
			SpellSkill3Projectile->SetOwner(this);
			SpellSkill3Projectile->SetActorRotation(CameraComponent->GetComponentRotation());

			GetWorldTimerManager().SetTimer(SpellSkill3DamageIncreaseTimer, FTimerDelegate::CreateLambda([&]()
			{			
				SpellSkill3Projectile->SetAttackDamage(SpellSkill2ProjectileIncreasedDamage);

				SpellSkill2ProjectileIncreasedDamage += 30.0f;

			}), 1.0f, true);

			FTimerHandle finishTimer;

			GetWorldTimerManager().SetTimer(finishTimer, FTimerDelegate::CreateLambda([&]()
			{
				if (GetWorldTimerManager().IsTimerActive(SpellSkill3DamageIncreaseTimer))
					GetWorldTimerManager().ClearTimer(SpellSkill3DamageIncreaseTimer);

			}), 6.5f, true);
			
			//if (WidgetComponent->GetHitResult().GetActor())
			//	WarriorSkill1Projectile->ShootProjectile(UKismetMathLibrary::GetDirectionUnitVector(GetMesh()->GetSocketLocation("Spawn_SpellSkill3_Projectile"), WidgetComponent->GetHitResult().ImpactPoint));
			//else
			//	WarriorSkill1Projectile->ShootProjectile(CameraComponent->GetForwardVector());
		}
	}
}

void ACPlayer::ShootProjectile()
{
	if (SpellSkill3ProjectileClass)
	{
		
		SpellSkill3Projectile = ACProjectile::SpawnProjectile(this, SpellSkill3ProjectileClass, FName("Spawn_SpellSkill3_Projectile"));
		ToggleSpellSkill3(true);
		
		if (SpellSkill3Projectile)
		{
			SpellSkill3Projectile->SetOwner(this);
			SpellSkill3Projectile->SetAttackDamage(SpellSkill2ProjectileIncreasedDamage);
			SpellSkill3Projectile->SetActorScale3D(SpellSkill2ProjectileFinalSize);
			SpellSkill3Projectile->SetActorRotation(CameraComponent->GetComponentRotation());

			SpellSkill3Projectile->ProjectileComponent->InitialSpeed = 5000.0f;
			SpellSkill3Projectile->ProjectileComponent->MaxSpeed = 5000.0f;

			if (WidgetComponent->GetHitResult().GetActor())
				SpellSkill3Projectile->ShootProjectile(UKismetMathLibrary::GetDirectionUnitVector(GetMesh()->GetSocketLocation("Spawn_SpellSkill3_Projectile"), WidgetComponent->GetHitResult().ImpactPoint));
			else
				SpellSkill3Projectile->ShootProjectile(CameraComponent->GetForwardVector());
		}
		
	}
}

void ACPlayer::SpawnSpellMeteorWeapon()
{	
	if (SpellMeteorClass)
	{
		SpellMeteorWeapon = ACWeapon::SpawnWeapon(this, SpellMeteorClass, Cast<ACCrosshair_SpellThrow>(Crosshair_SpellMeteor)->GetImpactPoint());
		
		if(SpellMeteorWeapon)
			SpellMeteorWeapon->SetOwner(this);		
	}
}

// MEMO: ���� ������ �����ͷ� �����µ� �� ����?
void ACPlayer::ShieldDefencing(ACEnemy* InAttacker)
{
	FVector target = InAttacker->GetActorLocation();

	FVector start = GetActorLocation();
	FVector direction = target - start;
	direction.Normalize();

	FTransform transform;
	transform.SetLocation(GetActorLocation());

	SetActorRotation(FRotator(GetActorRotation().Pitch, direction.Rotation().Yaw, GetActorRotation().Roll)/*UKismetMathLibrary::FindLookAtRotation(start, target)*/);
	LaunchCharacter(-direction * 500.0f, true, false);
	
	ActivateShieldDefenceEffect();

	if (ShieldDatas[0].Montage)
		ShieldDatas[0].PlayMontage(this);
}

void ACPlayer::SetPlayerPortalLocation()
{
	bPortalTeleporting = true;
	
	FVector portalLocation = PortalCrosshair->GetHitResult().ImpactPoint;
	
	CurrentCameraEffectType = ECameraEffectType::Teleport;

	SpawnCameraEffect();

	SetActorLocation(FVector(portalLocation.X + 20.0f, portalLocation.Y + 20.0f, portalLocation.Z + 150.0f));
}

void ACPlayer::MontageEnded(UAnimMontage* InMontage, bool Ininterrupted)
{
	// ��ų ����Ʈ ���� (�̺�Ʈ�� ���� �ɾ� ��)
	DestroySkillEffect();

	if (!bUseControllerRotationYaw)
		bUseControllerRotationYaw = true;

	if (IsDashing)
	{
		OffDash();

		IsDashing = false;
		IsMontagePlaying = false;

		return;
	}
	
	// ���� ���� ��
	//if (bUnequipping)
	//{
	//	CharacterComponent->SetCurrentWeaponType(EWeaponType::Unarmed);
	//
	//	bUnequipping = false;
	//	IsMontagePlaying = false;
	//
	//	return;
	//}

	// ���� ���� ��
	//if (bChanging)
	//{
	//	if (CharacterComponent->GetIsWeaponOnehandMode())
	//	{
	//		if (IsPressedOnSpell)
	//		{
	//			CharacterComponent->SetCurrentWeaponType(EWeaponType::Spell);
	//			CharacterComponent->GetEquipData(EWeaponType::Spell).PlayMontage(this);
	//			
	//			bChanging = false;
	//			IsPressedOnSpell = false;
	//			IsMontagePlaying = false;
	//
	//			return;
	//		}
	//		else if (IsPressedOnSpellFist)
	//		{
	//			CharacterComponent->SetCurrentWeaponType(EWeaponType::SpellFist);
	//			SpellFistEquipData[0].PlayMontage(this);
	//			
	//			bChanging = false;
	//			IsPressedOnSpellFist = false;
	//			IsMontagePlaying = false;
	//
	//			return;
	//		}
	//	}
	//	else if (CharacterComponent->GetIsWeaponSpellMode())
	//	{
	//		if (IsPressedOnOnehand)
	//		{
	//			CharacterComponent->SetCurrentWeaponType(EWeaponType::Onehand);
	//			CharacterComponent->GetEquipData(EWeaponType::Onehand).PlayMontage(this);
	//			
	//			bChanging = false;
	//			IsPressedOnOnehand = false;
	//			IsMontagePlaying = false;
	//
	//			return;
	//		}
	//		else if (IsPressedOnSpellFist)
	//		{
	//			CharacterComponent->SetCurrentWeaponType(EWeaponType::SpellFist);
	//			SpellFistEquipData[0].PlayMontage(this);
	//
	//			bChanging = false;
	//			IsPressedOnSpellFist = false;
	//			IsMontagePlaying = false;
	//
	//			return;
	//		}
	//	}
	//	else if (CharacterComponent->GetIsWeaponSpellFistMode())
	//	{
	//		if (IsPressedOnOnehand)
	//		{
	//			CharacterComponent->SetCurrentWeaponType(EWeaponType::Onehand);
	//			CharacterComponent->GetEquipData(EWeaponType::Onehand).PlayMontage(this);
	//
	//			bChanging = false;
	//			IsPressedOnOnehand = false;
	//			IsMontagePlaying = false;
	//
	//			return;
	//		}
	//		else if (IsPressedOnSpell)
	//		{
	//			CharacterComponent->SetCurrentWeaponType(EWeaponType::Spell);
	//			CharacterComponent->GetEquipData(EWeaponType::Spell).PlayMontage(this);
	//
	//			bChanging = false;
	//			IsPressedOnSpell = false;
	//			IsMontagePlaying = false;
	//
	//			return;
	//		}
	//	}
	//}

	// ���� Set
	{
		if (CharacterComponent->GetCurrentStateType() == EStateType::Attack)
			CharacterComponent->SetCurrentStateType(EStateType::Idle);

		if (bPortalTeleporting)
			bPortalTeleporting = false;

		if (!CharacterComponent->GetbCanMove())
			CharacterComponent->SetbCanMove(true);

		if (!CharacterComponent->GetbFixedCamera())
			CharacterComponent->SetbFixedCamera(true);

		if (IsActivateSkill)
			SetPlayerActivateSkill(false);
		
		bParkouring = false;

		IsMontagePlaying = false;

		GLog->Log("ACPlayer::OnMontageEnded()");
	}

	// Player�� ���� ���Ը�Ÿ�ְ� ��� ������ ����� �����Ǹ� �ٽ� OnAttack�Լ��� ȣ�Ⱑ���ϵ��� �����ؾߵ�
	// Player�� ���� ���̰� ���� ������� ���� ��Ÿ�ְ� ���ط� �����Ǿ��� ��� ����
	//if (Ininterrupted && bAttacking)
	//{
	//	bAttacking = false;
	//}
}

void ACPlayer::OnCollision()
{
	for (UShapeComponent* collision : CapsuleCollisions)
		collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ACPlayer::OffCollision()
{
	for (UShapeComponent* collision : CapsuleCollisions)
		collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACPlayer::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ICInterface_Item* itemInterface = Cast<ICInterface_Item>(OtherActor);

	if (itemInterface)
	{
		if (itemInterface->GetCurrentItemType() == EItemType::Hp)
			CharacterComponent->SetHp(itemInterface->ActivateItemAbility());
		else if (itemInterface->GetCurrentItemType() == EItemType::Mp)
			CharacterComponent->SetMp(itemInterface->ActivateItemAbility());
		else if (itemInterface->GetCurrentItemType() == EItemType::Sp)
			CharacterComponent->SetSp(itemInterface->ActivateItemAbility());	
	}
}

void ACPlayer::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ACPlayer::OnAttackBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CheckTrue(OverlappedComponent == GetCapsuleComponent());

	if (OtherActor && ((CharacterComponent->GetCurrentWeaponType() == EWeaponType::Onehand) || (CharacterComponent->GetCurrentWeaponType() == EWeaponType::Spell) || (CharacterComponent->GetCurrentWeaponType() == EWeaponType::Shield)))
	{
		ACEnemy* enemy = Cast<ACEnemy>(OtherActor);

		if (enemy)
			enemy->SetIsAttackByPlayer(true);
	}
}

void ACPlayer::OnAttackBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CheckTrue(OverlappedComponent == GetCapsuleComponent());

	if (OtherActor && ((CharacterComponent->GetCurrentWeaponType() == EWeaponType::Onehand) || (CharacterComponent->GetCurrentWeaponType() == EWeaponType::Spell) || (CharacterComponent->GetCurrentWeaponType() == EWeaponType::Shield)))
	{
		ACEnemy* enemy = Cast<ACEnemy>(OtherActor);

		if (enemy)
			enemy->SetIsAttackByPlayer(false);
	}
}

void ACPlayer::OnSphereSpellFistBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CheckTrue(OverlappedComponent == GetCapsuleComponent());
	
	GLog->Log("OnSphereSpellFist Overlapped!");

	if (OtherActor && (CharacterComponent->GetCurrentWeaponType() == EWeaponType::SpellFist))
	{
		ACEnemy* enemy = Cast<ACEnemy>(OtherActor);

		if (enemy)
			enemy->SetIsAttackByPlayer(true);
	}
}

void ACPlayer::OnSphereSpellFistEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CheckTrue(OverlappedComponent == GetCapsuleComponent());

	if (OtherActor && (CharacterComponent->GetCurrentWeaponType() == EWeaponType::SpellFist))
	{
		ACEnemy* enemy = Cast<ACEnemy>(OtherActor);

		if (enemy)
			enemy->SetIsAttackByPlayer(false);
	}
	//for (int i = 0; i < SpellFistedActors.Num(); i++)
	//{
	//	if (SpellFistedActors[i] == OtherActor)
	//		SpellFistedActors.Empty(i);
	//}
}

float ACPlayer::CurrentHp()
{
	return CharacterComponent->GetCurrentHp();
}

float ACPlayer::CurrentMp()
{
	return CharacterComponent->GetCurrentMp();
}

float ACPlayer::CurrentSp()
{
	return CharacterComponent->GetCurrentSp();
}

float ACPlayer::MaxHp()
{
	return CharacterComponent->GetMaxHp();
}

float ACPlayer::MaxMp()
{
	return CharacterComponent->GetMaxMp();
}

float ACPlayer::MaxSp()
{
	return CharacterComponent->GetMaxSp();
}

EPlayerNormalAttackType ACPlayer::GetCurrentPlayerNormalAttackType()
{
	return CurrentPlayerNormalAttackType;
}

EPlayerSkillType ACPlayer::GetCurrentPlayerSkillType()
{
	return CurrentPlayerSkillType;
}

EPlayerSpellFistType ACPlayer::GetCurrentPlayerSpellFistType()
{
	return CurrentPlayerSpellFistType;
}

bool ACPlayer::GetPlayerIsAttackByBoss()
{
	return IsAttackByBoss;
}

void ACPlayer::SetPlayerIsAttackByBoss(bool InBool)
{
	IsAttackByBoss = InBool;
}

void ACPlayer::SetPlayerIsInBossStage(bool InBool)
{
	IsAttackingBoss = InBool;
}

bool ACPlayer::GetPlayerUsingShield()
{
	return CharacterComponent->GetIsWeaponShieldMode();
}

bool ACPlayer::GetPlayerActivateSkill()
{
	return IsActivateSkill;
}

void ACPlayer::SetPlayerActivateSkill(bool InBool)
{
	IsActivateSkill = InBool;
}

// private: //////////////////////////////////////////////////////////////////////
void ACPlayer::OnMoveForward(float AxisValue)
{
	//if (ParkourComponent->IsClimbingMode())
	//{	
	//	FVector directionClimbUp = GetActorUpVector();
	//
	//	AddMovementInput(directionClimbUp, AxisValue);
	//	
	//	return;
	//}
	//CheckFalse(CharacterComponent->GetbCanMove());
	if (!IsParkouringClimbing)
	{
		CheckFalse(CharacterComponent->GetbCanMove());
		//CheckTrue(IsMontagePlaying);

		CharacterComponent->SetCurrentStateType(EStateType::Move);

		FRotator rotation = FRotator(0, GetControlRotation().Yaw, 0);
		FVector direction = FQuat(rotation).GetForwardVector().GetSafeNormal2D();

		AddMovementInput(direction, AxisValue);
	}
	else if (IsParkouringClimbing)
	{
		CharacterComponent->SetCurrentStateType(EStateType::Move);

		ClimbingInput = AxisValue;
			
		//AddMovementInput(direction, AxisValue);
	}
}

void ACPlayer::OnMoveRight(float AxisValue)
{
	//if (ParkourComponent->IsClimbingMode())
	//{
	//	FVector directionClimbRight = GetActorRightVector().GetSafeNormal2D();
	//	
	//	AddMovementInput(directionClimbRight, AxisValue);
	//
	//	return;
	//}
	CheckFalse(CharacterComponent->GetbCanMove());
	CheckTrue(ParkourComponent->IsClimbingMode());
	//CheckTrue(IsMontagePlaying);

	CharacterComponent->SetCurrentStateType(EStateType::Move);
	
	FRotator rotation = FRotator(0, GetControlRotation().Yaw, 0);
	FVector direction = FQuat(rotation).GetRightVector().GetSafeNormal2D();

	AddMovementInput(direction, AxisValue);
}

void ACPlayer::OnVerticalLook(float AxisValue)
{	
	if (!IsTargettingMode)
		AddControllerPitchInput(AxisValue);
}

void ACPlayer::OnHorizontalLook(float AxisValue)
{	
	if (!IsTargettingMode)
		AddControllerYawInput(AxisValue);
}

void ACPlayer::OnZoom(float AxisValue)
{
	if (!bAiming)
	{
		Zooming += (ZoomSpeed * AxisValue * GetWorld()->GetDeltaSeconds());
		Zooming = FMath::Clamp(Zooming, ZoomRange.X, ZoomRange.Y);
		
		CheckTrue(UKismetMathLibrary::NearlyEqual_FloatFloat(SpringArmComponent->TargetArmLength, Zooming));
		SpringArmComponent->TargetArmLength = UKismetMathLibrary::FInterpTo(SpringArmComponent->TargetArmLength, Zooming, GetWorld()->GetDeltaSeconds(), ZoomInterpSpeed);
	}
}

void ACPlayer::InZooming(float Infloat)
{
	CameraComponent->FieldOfView = Infloat;
}

void ACPlayer::OnAim()
{	
	bAiming = true;

	if (CharacterComponent->GetIsWeaponSpellMode())
	{
		PlaySpellPortalOnAimSound();

		WidgetComponent->SetbCrosshairVisible(false);

		PortalCrosshair = ACPortalCrosshair::SpawnPortalCrosshair(this, PortalCrosshairClass);
		PortalCrosshair->SetOwner(this);
		PortalCrosshair->SetActorRotation(CameraComponent->GetComponentRotation());

		//bUseControllerRotationYaw = true;
		//GetCharacterMovement()->bOrientRotationToMovement = false;
		
		SpringArmComponent->TargetArmLength = 95.0f;
		SpringArmComponent->SocketOffset = FVector(0, 30, 10);
		SpringArmComponent->bEnableCameraLag = false;

		Timeline.PlayFromStart();
	}
}

void ACPlayer::OffAim()
{
	if (CharacterComponent->GetIsWeaponSpellMode())
	{
		if(PortalCrosshair)
			PortalCrosshair->DestroyPortalCrosshair();

		WidgetComponent->SetbCrosshairVisible(true);

		//bUseControllerRotationYaw = false;
		//GetCharacterMovement()->bOrientRotationToMovement = true;

		Timeline.ReverseFromEnd();
		
		SpringArmComponent->TargetArmLength = 350.0f;
		SpringArmComponent->SocketOffset = FVector(0, 60, 0);
		SpringArmComponent->bEnableCameraLag = true;

		if (!CharacterComponent->GetIsMontagePlaying())
		{
			CharacterComponent->GetActionDatasSpell(2).PlayMontage(this);

			GLog->Log("ACPlayer::OnAction() SpellPortal");
		}

		PlaySpellPortalOffAimSound();
	}
	
	bAiming = false;
}

void ACPlayer::OnRun()
{
	GetCharacterMovement()->MaxWalkSpeed = 900.0;
}

void ACPlayer::OffRun()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.0;
}

void ACPlayer::OnJump()
{
	CheckFalse(CharacterComponent->GetbCanMove());
	CheckTrue(IsSpellTravel);
	//CheckTrue(IsAttackByBoss);
	
	CharacterComponent->SetCurrentStateType(EStateType::Move);
}

void ACPlayer::OffJump()
{
	CheckFalse(CharacterComponent->GetbCanMove());
	//CheckTrue(bAttacking);
	CheckTrue(IsSpellTravel);
	
	CharacterComponent->SetCurrentStateType(EStateType::Idle);
}

void ACPlayer::OnDash()
{	
	CheckTrue(GetCharacterMovement()->IsFalling());
	
	if (DashData[0].Montage)
	{
		IsDashing = true;
		
		DashData[0].PlayMontage(this);

		GetCharacterMovement()->MaxAcceleration = 4096.0f;
		GetCharacterMovement()->MaxWalkSpeed = 20000.0f;

		IsMontagePlaying = true;
	}
}

void ACPlayer::OffDash()
{
	GetCharacterMovement()->MaxAcceleration = 2048.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}

void ACPlayer::OnParkour()
{
	CheckTrue(IsMontagePlaying);
	CheckTrue(GetCharacterMovement()->IsFalling());
	CheckTrue(bParkouring);
	
	IsMontagePlaying = true;
	bParkouring = true;

	ParkourComponent->DoParkour();

	//if (ParkourComponent->IsRollMode())
		//ParkourTargetRot = GetLastMovementInputVector().Rotation();
}

void ACPlayer::OffParkour()
{
	//bParkouring = false;

	if (bAttacking)
		bAttacking = false;
}

void ACPlayer::OnOnehand()
{
	CheckTrue(IsMontagePlaying);

	// �̹� Onehand ������ ��� ���� ����
	if (CharacterComponent->GetIsWeaponOnehandMode())
	{
		bUnequipping = true;

		CharacterComponent->GetNormalWeapon(EWeaponType::Onehand)->AttachTo("Onehand_Holster");
		CharacterComponent->SetCurrentWeaponType(EWeaponType::Unarmed);

		bUnequipping = false;

		WeaponChanged();

		GLog->Log("ACPlayer::OnOnehand() Unequip");
		
		return;
	}

	// Spell Ȥ�� Spell Fist�� ��� �ٷ� Onehand ����
	if(CharacterComponent->GetIsWeaponSpellMode() || CharacterComponent->GetIsWeaponSpellFistMode())
	{
		bChanging = true;
		
		IsMontagePlaying = true;
		CharacterComponent->GetEquipData(EWeaponType::Onehand).PlayMontage(this);
		CharacterComponent->SetCurrentWeaponType(EWeaponType::Onehand);

		bChanging = false;

		WeaponChanged();
		
		GLog->Log("ACPlayer::OnOnehand() Change");

		return;
	}

	IsMontagePlaying = true;
	CharacterComponent->GetEquipData(EWeaponType::Onehand).PlayMontage(this);
	CharacterComponent->SetCurrentWeaponType(EWeaponType::Onehand);
	
	WeaponChanged();

	GLog->Log("ACPlayer::OnOnehand() Equip");
}

void ACPlayer::OnSpell()
{
	CheckTrue(IsMontagePlaying);
	
	// Spell�� ��� ���� ���� ����
	if (CharacterComponent->GetIsWeaponSpellMode())
	{
		bUnequipping = true;

		CharacterComponent->SetCurrentWeaponType(EWeaponType::Unarmed);

		WeaponChanged();

		bUnequipping = false;

		GLog->Log("ACPlayer::OnSpell() Unequip");

		return;
	}

	// Onehand�� ��� ���� ������ �����ϰ� Equip���� ��Ÿ�� ���� �� EWeaponType ����
	if (CharacterComponent->GetIsWeaponOnehandMode())
	{
		bChanging = true;

		CharacterComponent->GetNormalWeapon(EWeaponType::Onehand)->AttachTo("Onehand_Holster");
		IsMontagePlaying = true;
		CharacterComponent->GetEquipData(EWeaponType::Spell).PlayMontage(this);
		CharacterComponent->SetCurrentWeaponType(EWeaponType::Spell);
		
		WeaponChanged();

		bChanging = false;

		GLog->Log("ACPlayer::OnSpell() Change");

		return;
	}

	// Spell Fist �� ��� Equip ���� ��Ÿ�� ���� �� EWeaponType ����
	if (CharacterComponent->GetIsWeaponSpellFistMode())
	{
		bChanging = true;

		IsMontagePlaying = true;
		CharacterComponent->GetEquipData(EWeaponType::Spell).PlayMontage(this);
		CharacterComponent->SetCurrentWeaponType(EWeaponType::Spell);

		WeaponChanged();

		bChanging = false;

		GLog->Log("ACPlayer::OnOnehand() Change");

		return;
	}

	IsMontagePlaying = true;
	CharacterComponent->GetEquipData(EWeaponType::Spell).PlayMontage(this);
	CharacterComponent->SetCurrentWeaponType(EWeaponType::Spell);

	WeaponChanged();

	GLog->Log("ACPlayer::OnSpell() Equip");
}

void ACPlayer::OnSpellTravel()
{
	if (CharacterComponent->GetCurrentWeaponType() == EWeaponType::Spell)
	{
		if (!IsSpellTravel)
		{
			IsSpellTravel = true;
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
			SpawnTravelModeEffect();
			bUseControllerRotationPitch = true;
		}
		else if (IsSpellTravel)
		{
			IsSpellTravel = false;
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
			DestroyTravelModeEffect();
			bUseControllerRotationPitch = false;
		}
	}
}

void ACPlayer::OnSpellFist()
{
	CheckTrue(IsMontagePlaying);
	
	// SpellFist�̸� �ٷ� EWeaponType�� Unarmed�� ����
	if (CharacterComponent->GetIsWeaponSpellFistMode())
	{
		bUnequipping = true;

		CharacterComponent->SetCurrentWeaponType(EWeaponType::Unarmed);

		WeaponChanged();

		bUnequipping = false;

		GLog->Log("ACPlayer::OnOnehand() Change");

		return;
	}

	// Onehand�̸� ���� ���� ���� �� Equip��Ÿ�� ���� �� EWeaponType ����
	if (CharacterComponent->GetIsWeaponOnehandMode())
	{
		bChanging = true;

		CharacterComponent->GetNormalWeapon(EWeaponType::Onehand)->AttachTo("Onehand_Holster");
		IsMontagePlaying = true;
		SpellFistEquipData[0].PlayMontage(this);
		CharacterComponent->SetCurrentWeaponType(EWeaponType::SpellFist);

		WeaponChanged();

		bChanging = false;

		GLog->Log("ACPlayer::OnSpell() Change");

		return;
	}

	// Spell�̸� �ٷ� Equip ��Ÿ�� ���� �� EWeaponType ����
	if (CharacterComponent->GetIsWeaponSpellMode())
	{
		bChanging = true;

		IsMontagePlaying = true;
		SpellFistEquipData[0].PlayMontage(this);
		CharacterComponent->SetCurrentWeaponType(EWeaponType::SpellFist);

		WeaponChanged();

		bChanging = false;

		GLog->Log("ACPlayer::OnSpell() Unequip");

		return;
	}

	IsMontagePlaying = true;
	SpellFistEquipData[0].PlayMontage(this);
	CharacterComponent->SetCurrentWeaponType(EWeaponType::SpellFist);

	WeaponChanged();

	GLog->Log("ACPlayer::OnSpell() Equip");
}

void ACPlayer::OnSpawnFriend()
{
	if (CharacterComponent->GetIsWeaponOnehandMode())
	{
		if (SpawnDatas[0].Montage)
		{
			SpawnDatas[0].PlayMontage(this);

			if (OnSpawnPlayerFriend.IsBound())
				OnSpawnPlayerFriend.Broadcast();
		}
	}
	else if (CharacterComponent->GetIsWeaponSpellMode())
	{
		if (SpawnDatas[1].Montage)
		{
			SpawnDatas[1].PlayMontage(this);

			if (OnSpawnPlayerFriend.IsBound())
				OnSpawnPlayerFriend.Broadcast();
		}
	}
	else if (CharacterComponent->GetIsWeaponSpellFistMode())
	{
		if (SpawnDatas[2].Montage)
		{
			SpawnDatas[2].PlayMontage(this);

			//if (OnSpawnPlayerFriend.IsBound())
			//	OnSpawnPlayerFriend.Broadcast();
		}
	}
}

void ACPlayer::OnShield()
{
	CheckFalse(CharacterComponent->GetIsWeaponOnehandMode());

	ActivateShieldEquipEffect();
	CharacterComponent->SetCurrentWeaponType(EWeaponType::Shield);
	
	
	//// Shield ���� ���� ��Ÿ��
	//if (ShieldDatas[0].Montage)
	//	ShieldDatas[0].PlayMontage(this);

	GLog->Log("ACPlayer::On Shield()");
}

void ACPlayer::OffShield()
{
	CheckFalse(CharacterComponent->GetIsWeaponShieldMode());

	//SpringArmComponent->TargetArmLength = 350.0f;
	ActivateShieldUnequipEffect();
	CharacterComponent->SetCurrentWeaponType(EWeaponType::Onehand);

	GLog->Log("Off Shield");
}

void ACPlayer::OnAction()
{
	CheckFalse(CanOnAction);
	CheckTrue(CharacterComponent->GetIsWeaponUnarmedMode());
	CheckTrue(bAiming);
	//CheckTrue(IsMontagePlaying);
	CheckTrue(bParkouring);

	CharacterComponent->SetCurrentStateType(EStateType::Attack);
	// MEMO: �� Setter�� Normal Attack�� Skill Attack �� �� ��� (���) Enemy Strafing �����ϸ� �� �� �ڿ������� �� ����
	//SetPlayerActivateSkill(true);

	if (CharacterComponent->GetIsWeaponOnehandMode())
	{
		//if (GetMovementComponent()->IsFalling())
		//{
		//	//bAttacking = true;
		//
		//	CharacterComponent->GetActionDatasOnehand(3).PlayMontage(this);
		//
		//	return;
		//}
		
		if (Index >= 3)
			Index = 0;
		
		if (bCanCombo)
		{
			bCanNextAction = true;
			bCanCombo = false;

			return;
		}

		if (!bAttacking)
		{
			bAttacking = true;

			//if (!IsAttackingBoss)
			//{
				//CharacterComponent->GetActionDatasOnehand(Index).PlayMontage(this);
				//GLog->Log("ACPlayer::OnAction() NormalOnehandAttack");
			//}
			//else if (IsAttackingBoss)
			//{
				CharacterComponent->GetActionDatasOnehand(Index).PlayMontage(this);

				CharacterComponent->SetSp(-500.0f);

				IsMontagePlaying = true;
				//GLog->Log("ACPlayer::OnAction() NormalBossOnehandAttack");
			//}
		}

	}	
	else if (CharacterComponent->GetIsWeaponSpellMode())
	{
		CheckTrue(IsMontagePlaying);
		
		IsMontagePlaying = true;

		int select = UKismetMathLibrary::RandomIntegerInRange(0, 1);
		
		CharacterComponent->GetActionDatasSpell(select).PlayMontage(this);

		CharacterComponent->SetSp(-500.0f);

		GLog->Log("ACPlayer::OnAction() SpellAttackNormal");
		
		
	}
	else if (CharacterComponent->GetIsWeaponSpellFistMode())
	{
		if (Index >= 3)
			Index = 0;

		if (bCanCombo)
		{
			bCanNextAction = true;
			bCanCombo = false;

			return;
		}

		if (!bAttacking)
		{
			//CharacterComponent->SetCurrentStateType(EStateType::Attack);
			/* BUG: Ŭ���� ������ ��� �� ������ �ٲٴϱ� �� �۵��� �Ѵ�.
				 * �Ƹ��� �ð� ���⵵? �� �׷� ���� ������ �� �κп� ������ �ȉ�µ� ���͹��ȴ� �� ����.
				 * 1�� ° ���ݵ� ������ Ŭ�� �� ������ �ٽ� �����ϴ� ������ ���� Ȯ���Ͽ� �����Ͽ���. */
			bAttacking = true;
			SpellFistDatas[Index].PlayMontage(this);

			IsMontagePlaying = true;
			GLog->Log("ACPlayer::OnAction() SpellFistAttack");
		}
	}
}

void ACPlayer::OnSkillOne()
{
	if (!CanOnSkill1)
		WarningText(EWarningTextType::SkillCoolTimeActivate);
	if (CharacterComponent->GetCurrentMp() < 500.0f)
		WarningText(EWarningTextType::SkillMPShortage);

	CheckTrue(IsMontagePlaying);
	CheckFalse(CanOnSkill1);
	
	if (CharacterComponent->GetCurrentMp() < 500.0f)
		return;

	SetPlayerActivateSkill(true);

	IsMontagePlaying = true;
	
	CharacterComponent->SetMp(-500.0f);

	if (CharacterComponent->GetIsWeaponOnehandMode())
	{	
		CharacterComponent->SetCurrentStateType(EStateType::Attack);


		if (CharacterComponent->GetCriticalDatasOnehand(0).Montage)
		{
			CharacterComponent->GetCriticalDatasOnehand(0).PlayMontage(this);
			ActivateSkillCoolTime(CharacterComponent->GetCurrentWeaponType(), ESkillType::Skill1);
			IsMontagePlaying = true;
		}

		//GetWorldTimerManager().SetTimer(WarriorSkillTimer, this, &ACPlayer::SpawnWarriorSkillOneProjectile, 0.3f, true);
		
		//CharacterComponent->SetIsMontagePlaying(true);
	}
	else if (CharacterComponent->GetIsWeaponSpellMode())
	{	
		bAiming = true;

		CharacterComponent->SetCurrentStateType(EStateType::Attack);
		WidgetComponent->SetbCrosshairVisible(false);
		Crosshair_SpellMeteor = ACCrosshair::SpawnCrosshair(this, Crosshair_SpellMeteorClass);
		Crosshair_SpellMeteor->SetOwner(this);

		SpringArmComponent->bEnableCameraLag = false;
		SpringArmComponent->TargetArmLength = 600.0f;
		SpringArmComponent->SocketOffset = FVector(0.0f, 60.0f, 250.0f);
		
		Timeline.ReverseFromEnd();

		bOnCriticalReady = true;
	}
	else if (CharacterComponent->GetIsWeaponSpellFistMode())
	{
		CharacterComponent->SetCurrentStateType(EStateType::Attack);

		if (SpellFistSkillDatas[0].Montage)
		{
			SpellFistSkillDatas[0].PlayMontage(this);
			ActivateSkillCoolTime(CharacterComponent->GetCurrentWeaponType(), ESkillType::Skill1);
			IsMontagePlaying = true;
		}
	}
}

void ACPlayer::OffSkillOne()
{
	if (CharacterComponent->GetIsWeaponSpellMode())
	{
		if (!IsActivateSkill)
			return;
		
		if (Crosshair_SpellMeteor)
			Crosshair_SpellMeteor->DestroyCrosshair();
		
		WidgetComponent->SetbCrosshairVisible(true);

		//CharacterComponent->SetIsMontagePlaying(true);
		IsMontagePlaying = true;
		CharacterComponent->SetCurrentStateType(EStateType::Attack);

		CharacterComponent->GetCriticalDatasSpell(0).PlayMontage(this);
		ActivateSkillCoolTime(CharacterComponent->GetCurrentWeaponType(), ESkillType::Skill1);

		SpringArmComponent->TargetArmLength = 350.0f;
		SpringArmComponent->SocketOffset = FVector(0.0f, 60.0f, 0.0f);
		SpringArmComponent->bEnableCameraLag = true;
		
		Timeline.PlayFromStart();

		bOnCriticalReady = false;

		bAiming = false;
	}
}

void ACPlayer::OnSkillTwo()
{
	if (!CanOnSkill2)
		WarningText(EWarningTextType::SkillCoolTimeActivate);
	if (CharacterComponent->GetCurrentMp() < 500.0f)
		WarningText(EWarningTextType::SkillMPShortage);

	CheckTrue(IsMontagePlaying);
	CheckFalse(CanOnSkill2);

	if (CharacterComponent->GetCurrentMp() < 500.0f)
		return;

	SetPlayerActivateSkill(true);
	
	CharacterComponent->SetMp(-500.0f);

	if (CharacterComponent->GetIsWeaponOnehandMode())
	{
		CharacterComponent->SetCurrentStateType(EStateType::Attack);
		//CharacterComponent->SetIsMontagePlaying(true);
		IsMontagePlaying = true;

		if (CharacterComponent->GetCriticalDatasOnehand(1).Montage)
		{
			//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
			CharacterComponent->GetCriticalDatasOnehand(1).PlayMontage(this);
			ActivateSkillCoolTime(CharacterComponent->GetCurrentWeaponType(), ESkillType::Skill2);
		}
	}
	else if (CharacterComponent->GetIsWeaponSpellMode())
	{
		CharacterComponent->SetCurrentStateType(EStateType::Attack);
		//CharacterComponent->SetIsMontagePlaying(true);
		IsMontagePlaying = true;

		if (CharacterComponent->GetCriticalDatasSpell(1).Montage)
		{
			CharacterComponent->GetCriticalDatasSpell(1).PlayMontage(this);
			ActivateSkillCoolTime(CharacterComponent->GetCurrentWeaponType(), ESkillType::Skill2);
		}
	}
	else if (CharacterComponent->GetIsWeaponSpellFistMode())
	{
		CharacterComponent->SetCurrentStateType(EStateType::Attack);
		IsMontagePlaying = true;

		if (SpellFistSkillDatas[1].Montage)
		{
			SpellFistSkillDatas[1].PlayMontage(this);
			ActivateSkillCoolTime(CharacterComponent->GetCurrentWeaponType(), ESkillType::Skill2);
		}
	}
}

void ACPlayer::OnSkillThree()
{
	if (!CanOnSkill3)
		WarningText(EWarningTextType::SkillCoolTimeActivate);
	if (CharacterComponent->GetCurrentMp() < 500.0f)
		WarningText(EWarningTextType::SkillMPShortage);

	CheckTrue(IsMontagePlaying);
	CheckFalse(CanOnSkill3);
	
	if (CharacterComponent->GetCurrentMp() < 500.0f)
		return;

	CharacterComponent->SetMp(-500.0f);
	
	if (CharacterComponent->GetIsWeaponOnehandMode())
	{
		CharacterComponent->SetCurrentStateType(EStateType::Attack);

		IsMontagePlaying = true;

		if (CharacterComponent->GetCriticalDatasOnehand(2).Montage)
		{
			SetPlayerActivateSkill(true);
			//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
			CharacterComponent->GetCriticalDatasOnehand(2).PlayMontage(this);
			ActivateSkillCoolTime(CharacterComponent->GetCurrentWeaponType(), ESkillType::Skill3);
		}
	}
	else if (CharacterComponent->GetIsWeaponSpellMode())
	{
		if (CharacterComponent->GetCriticalDatasSpell(2).Montage)
		{
			IsMontagePlaying = true;
			
			CharacterComponent->GetCriticalDatasSpell(2).PlayMontage(this);
			ActivateSkillCoolTime(CharacterComponent->GetCurrentWeaponType(), ESkillType::Skill3);

			ToggleSpellSkill3Effect(true);
			
			SpawnProjectile();

			//if (OnSpawnSpellSkill2Projectile.IsBound())
			//	OnSpawnSpellSkill2Projectile.Broadcast();
		}
	}
	else if (CharacterComponent->GetIsWeaponSpellFistMode())
	{
		IsTargettingMode = true;
		ToggleLockOn();

		SetPlayerActivateSkill(true);		
	}
}

void ACPlayer::OffSkillThree()
{
	if (CharacterComponent->GetIsWeaponSpellMode())
	{
		ActivateSkillCoolTime(CharacterComponent->GetCurrentWeaponType(), ESkillType::Skill3);
		
		IsSpell3ProjectileSpawned = false;
		
		// OnSkillThree()���� Spawn�� Projectile �Ҹ��Ŵ
		if (SpellSkill3Projectile)
			SpellSkill3Projectile->Destroy();

		// Spawn�Ǿ��� Projectile Effect �Ҹ��Ŵ
		ToggleSpellSkill3Effect(false);

		if (GetWorldTimerManager().IsTimerActive(SpellSkill3DamageIncreaseTimer))
			GetWorldTimerManager().ClearTimer(SpellSkill3DamageIncreaseTimer);

		if (CharacterComponent->GetCriticalDatasSpell(3).Montage)
		{
			CharacterComponent->GetCriticalDatasSpell(3).PlayMontage(this);
		}
		
		IsSpell3ProjectileShoot = true;

		GetWorldTimerManager().SetTimer(SpellSkill2Timer, this, &ACPlayer::ShootProjectile, 0.2f, true);

		FTimerHandle finishTimer;

		GetWorldTimerManager().SetTimer(finishTimer, FTimerDelegate::CreateLambda([&]()
		{
			GetWorldTimerManager().ClearTimer(SpellSkill2Timer);
			
			ToggleSpellSkill3(false);

			IsSpell3ProjectileShoot = false;
			SpellSkill2ProjectileIncreasedDamage = 100.0f;
		}), 11.0f, false);

		IsMontagePlaying = false;
	}
	else if (CharacterComponent->GetIsWeaponSpellFistMode())
	{
		IsTargettingMode = false;
		ToggleLockOn();

		CurrentCameraEffectType = ECameraEffectType::Teleport;
		SpawnCameraEffect();

		//SetActorLocation(LockedOnTargetLocation);

		if (SpellFistSkillDatas[2].Montage)
			SpellFistSkillDatas[2].PlayMontage(this);

		if (LockedOnTargetLocation != FVector::ZeroVector)
		{
			FLatentActionInfo info;
			info.CallbackTarget = this;

			UKismetSystemLibrary::MoveComponentTo
			(
				RootComponent,
				LockedOnTargetLocation,
				GetActorRotation(),
				true,
				false,
				SpellSkill3MovingTime,
				false,
				EMoveComponentAction::Move,
				info
			);

			CurrentCameraEffectType = ECameraEffectType::Max;
		}
	}
}

void ACPlayer::OnCritical()
{
	//SpawnSkillEffect();
	
	if (CharacterComponent->GetIsWeaponOnehandMode())
	{
		CharacterComponent->SetCurrentStateType(EStateType::Attack);
		
		if (CharacterComponent->GetCriticalDatasOnehand(3).Montage)
			CharacterComponent->GetCriticalDatasOnehand(3).PlayMontage(this);
	}
	else if (CharacterComponent->GetIsWeaponSpellMode())
	{
		//CharacterComponent->SetCurrentStateType(EStateType::Attack);
	}
}

void ACPlayer::ShakeCamera()
{
	if (GetWorld()->GetFirstPlayerController() && DamageCameraShakeClass)
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(DamageCameraShakeClass);
}

bool ACPlayer::OnActionChecker()
{
	if (CharacterComponent->GetCurrentSp() < 500.0f) 
		return false;
	else
		return true;
}

void ACPlayer::OnSkill1Checker(bool CanUseSkill)
{
	CanOnSkill1 = CanUseSkill;
}

void ACPlayer::OnSkill2Checker(bool CanUseSkill)
{
	CanOnSkill2 = CanUseSkill;
}

void ACPlayer::OnSkill3Checker(bool CanUseSkill)
{
	CanOnSkill3 = CanUseSkill;
}

void ACPlayer::GetDamageData(int32 InIndex)
{
	if (DamageDatas[InIndex].Montage)
		DamageDatas[InIndex].PlayMontage(this);
	
	if (DamageDatas[InIndex].Effect)
		DamageDatas[InIndex].PlayEffect(GetWorld(), this);
}

FVector ACPlayer::CalculateMeshSocketToVectorLocation(FName InSocketName, FVector InDirectionTo)
{
	return UKismetMathLibrary::GetDirectionUnitVector(GetMesh()->GetSocketLocation(InSocketName), InDirectionTo);
}

void ACPlayer::Debug_PlayerLog()
{
	//if (StateComponent)
	//{
	//	EStateType type = StateComponent->GetType();
	//	FString typeToString = "Max";

	//	if (type == EStateType::Max)
	//		typeToString = "Max";
	//	if (type == EStateType::Idle)
	//		typeToString = "Idle";
	//	if (type == EStateType::Equip)
	//		typeToString = "Equip";
	//	if (type == EStateType::Move)
	//		typeToString = "Move";
	//	if (type == EStateType::Parkour)
	//		typeToString = "Parkour";
	//	if (type == EStateType::AttackNormal)
	//		typeToString = "AttackNormal";
	//	if (type == EStateType::AttackCritical)
	//		typeToString = "AttackCritical";
	//	if (type == EStateType::AttackSkill)
	//		typeToString = "AttackSkill";
	//	if (type == EStateType::Block)
	//		typeToString = "Block";
	//	if (type == EStateType::Damage)
	//		typeToString = "Damage";
	//	if (type == EStateType::Dead)
	//		typeToString = "Dead";

	//	CLog::Print("///////////// Player Component Type /////////////", 1);
	//	CLog::Print("Current State Type: " + typeToString, 2);
	//}

	////if (WeaponComponent)
	////{
	//	//EWeaponType type = //WeaponComponent->GetType();
	//	FString typeToString = "Max";

	//	if (type == EWeaponType::Max)
	//		typeToString = "Max";
	//	if (type == EWeaponType::Onehand)
	//		typeToString = "Onehand";
	//	if (type == EWeaponType::Twohand)
	//		typeToString = "Twohand";
	//	if (type == EWeaponType::Rifle)
	//		typeToString = "Rifle";
	//	if (type == EWeaponType::Spell)
	//		typeToString = "Spell";

	//	CLog::Print("Current Weapon Type: " + typeToString, 3);
	//}

	//CLog::Print("///////////// Player Current Bool Type /////////////");
	//	
	//FString btype;

	//if (bEquipping)
	//	btype = "true";
	//else
	//	btype = "false";

	//CLog::Print("Current bEquipping Type: " + btype, 4);
	
}
 
void ACPlayer::Debug_OnControllerRotationYaw()


{
	if (bUseControllerRotationYaw)
		bUseControllerRotationYaw = false;
	else
		bUseControllerRotationYaw = true;
}

// protected: //////////////////////////////////////////////////////////////////////
void ACPlayer::BeginPlay()
{
	Super::BeginPlay();

	Zooming = SpringArmComponent->TargetArmLength;

	GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &ACPlayer::MontageEnded);

	GetComponents<UCapsuleComponent>(CapsuleCollisions);
	
	BoxComponentSkill->OnComponentBeginOverlap.AddDynamic(this, &ACPlayer::OnAttackBoxBeginOverlap);
	BoxComponentSkill->OnComponentEndOverlap.AddDynamic(this, &ACPlayer::OnAttackBoxEndOverlap);

	//BoxComponentSkill3->OnComponentBeginOverlap.AddDynamic(this, &ACPlayer::OnBoxSkill3BeginOverlap);
	//BoxComponentSkill3->OnComponentEndOverlap.AddDynamic(this, &ACPlayer::OnBoxSkill3EndOverlap);
	//
	SphereComponentSpellFist->OnComponentBeginOverlap.AddDynamic(this, &ACPlayer::OnSphereSpellFistBeginOverlap);
	SphereComponentSpellFist->OnComponentEndOverlap.AddDynamic(this, &ACPlayer::OnSphereSpellFistEndOverlap);
	
	for (UShapeComponent* collision : CapsuleCollisions)
	{
		collision->OnComponentBeginOverlap.AddDynamic(this, &ACPlayer::OnBeginOverlap);
		collision->OnComponentEndOverlap.AddDynamic(this, &ACPlayer::OnEndOverlap);
	}

	if (SpellFistWeaponClass)
	{
		FActorSpawnParameters params;
		params.Owner = this;
		
		SpellFistWeapon = GetWorld()->SpawnActor<ACWeapon>(SpellFistWeaponClass, params);
	}

	CharacterComponent->SetCurrentWeaponType(EWeaponType::Unarmed);
	
	OnTimelineFloat.BindUFunction(this, "InZooming");
	Timeline.AddInterpFloat(AimCurve, OnTimelineFloat);
	Timeline.SetPlayRate(200);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	Notify_SetCurrentPlayerSkillType(EPlayerSkillType::Max);
	Notify_SetCurrentPlayerSpellFistType(EPlayerSpellFistType::Max);
	
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Enemy", SpellFistedActors);

	WarningText(EWarningTextType::Max);

	// �ӽ�
	//FTimerHandle timer;

	//GetWorldTimerManager().SetTimer(timer, this, &UCCharacterComponent::SetSp(100.0f), )
}