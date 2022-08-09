/* MEMO: CPlayer.cpp 코드 배치 정리 완료 
* .h 파일에 선언된 함수 원형 순으로 함수 정의를 작성하였다. */
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
	}

	{
		//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);//
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bUseControllerRotationYaw = true;
	}

	//{
	//	const UEnum* ptr = FindObject<UEnum>(ANY_PACKAGE, L"EParkourArrowType", true);
	//
	//	for (int32 i = 0; i < (int32)EParkourArrowType::Max; i++)
	//	{
	//		FName name = FName(ptr->GetNameStringByIndex(i));
	//		CHelpers::CreateComponent<UArrowComponent>(this, &Arrows[i], name, ArrowGroup);
	//
	//		switch ((EParkourArrowType)i)
	//		{
	//		case EParkourArrowType::Center:
	//			Arrows[i]->ArrowColor = FColor::Red;
	//
	//			break;
	//
	//		case EParkourArrowType::Ceil:
	//			Arrows[i]->ArrowColor = FColor::Green;
	//			Arrows[i]->SetRelativeLocation(FVector(0, 0, 100));
	//
	//			break;
	//
	//		case EParkourArrowType::Floor:
	//			Arrows[i]->ArrowColor = FColor::Blue;
	//			Arrows[i]->SetRelativeLocation(FVector(0, 0, -80));
	//
	//			break;
	//
	//		case EParkourArrowType::Left:
	//			Arrows[i]->ArrowColor = FColor::Magenta;
	//			Arrows[i]->SetRelativeLocation(FVector(0, -30, 0));
	//
	//			break;
	//
	//		case EParkourArrowType::Right:
	//			Arrows[i]->ArrowColor = FColor::Magenta;
	//			Arrows[i]->SetRelativeLocation(FVector(0, 30, 0));
	//
	//			break;
	//
	//		case EParkourArrowType::Land:
	//			Arrows[i]->ArrowColor = FColor::Red;
	//			Arrows[i]->SetRelativeLocation(FVector(0, 0, -80));
	//			Arrows[i]->SetRelativeRotation(FRotator(-90, 0, 0));
	//
	//			break;
	//		}
	//	}
	//}

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
	
}

void ACPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CanOnAction = OnActionChecker();

	Timeline.TickTimeline(DeltaTime);

	if (bParkouring)
		SetActorRotation(UKismetMathLibrary::RInterpTo(GetActorRotation(), ParkourTargetRot, GetWorld()->GetDeltaSeconds(), 10.0f));
	


#ifdef LOG_PLAYER
	PlayerLog();
#endif 
}

void ACPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
		
	PlayerInputComponent->BindAxis("MoveForward", this, &ACPlayer::OnMoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACPlayer::OnMoveRight);
	PlayerInputComponent->BindAxis("VerticalLook", this, &ACPlayer::OnVerticalLook);
	PlayerInputComponent->BindAxis("HorizontalLook", this, &ACPlayer::OnHorizontalLook);
	PlayerInputComponent->BindAxis("Zoom", this, &ACPlayer::OnZoom);
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
}

float ACPlayer::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Damaged.DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser); // damage float
	//Damaged.DamageEvent = (FActionDamageEvent*)&DamageEvent;  // Hit 에니메이션 몽타주
	Damaged.EventInstigator = EventInstigator; // Player 이므로 Enemy controller
	Damaged.DamageCauser = DamageCauser; // weapon
	Damaged.DamageAmount = DamageAmount;
	
	CurrentCameraEffectType = ECameraEffectType::Damage;
	CharacterComponent->SetHp(-Damaged.DamageAmount);

	if (bParkouring)
		return 0.0f;

	CharacterComponent->SetCurrentStateType(EStateType::Damage);
	CurrentCameraEffectType = ECameraEffectType::Damage;
	//CharacterComponent->SetIsMontagePlaying(true);

	// Player 회전 & Knockback
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
			LaunchCharacter(-direction * 1200.0f, true, false);
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
		CharacterComponent->SetHp(-20.0f);
		CharacterComponent->SetCurrentStateType(EStateType::Damage);
		CurrentCameraEffectType = ECameraEffectType::Damage;
		
		ActivateDamageEffect();
		SpawnCameraEffect();
		ShakeCamera();
		
		CheckTrue(bParkouring);

		// Player 피격시 회전과 넉백 설정
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
				LaunchCharacter(-direction * 2500.0f, true, false);
			}
		}

		if (InType == EBossAttackType::NormalAttack)
		{
			GetDamageData(0);

			IsMontagePlaying = true;
		}
		else if (InType == EBossAttackType::BoundUpAttack)
		{
			GetDamageData(1);

			IsMontagePlaying = true;
		}
		else if (InType == EBossAttackType::GroggyAttack)
		{
			GetDamageData(2);

			IsMontagePlaying = true;
		}
		else if (InType == EBossAttackType::FinalAttack)
		{
			GetDamageData(3);

			IsMontagePlaying = true;
		}
	}
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

void ACPlayer::SpawnWarriorSkillOneProjectile()
{
	if (WarriorSkill1ProjectileClass)
	{
		SpawnWarriorSkillOneEffect();

		WarriorSkill1Projectile = ACProjectile::SpawnProjectile(this, WarriorSkill1ProjectileClass, FName("Spawn_Player_Warrior_Skill1_Projectile"));
		WarriorSkill1Projectile->SetOwner(this);
		WarriorSkill1Projectile->SetActorRotation(CameraComponent->GetComponentRotation());

		if (WidgetComponent->GetHitResult().GetActor())
			WarriorSkill1Projectile->ShootProjectile(UKismetMathLibrary::GetDirectionUnitVector(GetMesh()->GetSocketLocation("Spawn_Player_Warrior_Skill1_Projectile"), WidgetComponent->GetHitResult().ImpactPoint));
		else
			WarriorSkill1Projectile->ShootProjectile(CameraComponent->GetForwardVector());
	}
}

void ACPlayer::SpawnSpellMeteorWeapon()
{
	SpellMeteorWeapon = ACWeapon::SpawnWeapon(this, SpellMeteorClass, Cast<ACCrosshair_SpellThrow>(Crosshair_SpellMeteor)->GetImpactPoint());
	
	if(SpellMeteorWeapon)
		SpellMeteorWeapon->SetOwner(this);		
}

// MEMO: 지역 변수가 포인터로 들어오는데 왜 되지?
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
	// 스킬 이펙트 해제 이벤트에 조건 걸어 둠
	DestroySkillEffect();
	
	// 무기 해제 중
	if (bUnequipping)
	{
		CharacterComponent->SetCurrentWeaponType(EWeaponType::Unarmed);

		bUnequipping = false;
		IsMontagePlaying = false;

		return;
	}

	// 무기 변경 중
	if (bChanging)
	{
		if (CharacterComponent->GetIsWeaponOnehandMode())
		{
			if (IsPressedOnSpell)
			{
				CharacterComponent->SetCurrentWeaponType(EWeaponType::Spell);
				CharacterComponent->GetEquipData(EWeaponType::Spell).PlayMontage(this);
				
				bChanging = false;
				IsPressedOnSpell = false;
				IsMontagePlaying = false;

				return;
			}
			else if (IsPressedOnSpellFist)
			{
				CharacterComponent->SetCurrentWeaponType(EWeaponType::SpellFist);
				SpellFistEquipData[0].PlayMontage(this);
				
				bChanging = false;
				IsPressedOnSpellFist = false;
				IsMontagePlaying = false;

				return;
			}
		}
		else if (CharacterComponent->GetIsWeaponSpellMode())
		{
			if (IsPressedOnOnehand)
			{
				CharacterComponent->SetCurrentWeaponType(EWeaponType::Onehand);
				CharacterComponent->GetEquipData(EWeaponType::Onehand).PlayMontage(this);
				
				bChanging = false;
				IsPressedOnOnehand = false;
				IsMontagePlaying = false;

				return;
			}
			else if (IsPressedOnSpellFist)
			{
				CharacterComponent->SetCurrentWeaponType(EWeaponType::SpellFist);
				SpellFistEquipData[0].PlayMontage(this);

				bChanging = false;
				IsPressedOnSpellFist = false;
				IsMontagePlaying = false;

				return;
			}
		}
		else if (CharacterComponent->GetIsWeaponSpellFistMode())
		{
			if (IsPressedOnOnehand)
			{
				CharacterComponent->SetCurrentWeaponType(EWeaponType::Onehand);
				CharacterComponent->GetEquipData(EWeaponType::Onehand).PlayMontage(this);

				bChanging = false;
				IsPressedOnOnehand = false;
				IsMontagePlaying = false;

				return;
			}
			else if (IsPressedOnSpell)
			{
				CharacterComponent->SetCurrentWeaponType(EWeaponType::Spell);
				CharacterComponent->GetEquipData(EWeaponType::Spell).PlayMontage(this);

				bChanging = false;
				IsPressedOnSpell = false;
				IsMontagePlaying = false;

				return;
			}
		}
	}

	// 공통 Set
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
		
		IsMontagePlaying = false;

		GLog->Log("ACPlayer::OnMontageEnded()");
	}

	// Player의 공격 에님몽타주가 어떠한 이유로 재생이 정지되면 다시 OnAttack함수를 호출가능하도록 설정해야됨
	// Player가 공격 중이고 현재 재생중인 에님 몽타주가 방해로 정지되었을 경우 실행
	if (Ininterrupted && bAttacking)
	{
		bAttacking = false;
	}
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

	if (OtherActor)
	{
		ACEnemy* enemy = Cast<ACEnemy>(OtherActor);

		if (enemy)
		{
			enemy->SetIsAttackByPlayer(true);

		}
	}
}

void ACPlayer::OnAttackBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CheckTrue(OverlappedComponent == GetCapsuleComponent());

	if (OtherActor)
	{
		ACEnemy* enemy = Cast<ACEnemy>(OtherActor);

		if (enemy)
		{
			enemy->SetIsAttackByPlayer(false);
		}
	}
}

void ACPlayer::OnSphereSpellFistBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CheckTrue(OverlappedComponent == GetCapsuleComponent());
	
	if (OtherActor)
	{
		ACEnemy* enemy = Cast<ACEnemy>(OtherActor);

		if (enemy)
			enemy->SetIsAttackBySpellFist(true);
	}
}

void ACPlayer::OnSphereSpellFistEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CheckTrue(OverlappedComponent == GetCapsuleComponent());


	if (OtherActor)
	{
		ACEnemy* enemy = Cast<ACEnemy>(OtherActor);

		if (enemy)
			enemy->SetIsAttackBySpellFist(false);
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
	CheckFalse(CharacterComponent->GetbCanMove());
	//CheckTrue(IsMontagePlaying);

	CharacterComponent->SetCurrentStateType(EStateType::Move);

	FRotator rotation = FRotator(0, GetControlRotation().Yaw, 0);
	FVector direction = FQuat(rotation).GetForwardVector().GetSafeNormal2D();

	AddMovementInput(direction, AxisValue);
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
	//CheckTrue(IsMontagePlaying);

	CharacterComponent->SetCurrentStateType(EStateType::Move);
	
	FRotator rotation = FRotator(0, GetControlRotation().Yaw, 0);
	FVector direction = FQuat(rotation).GetRightVector().GetSafeNormal2D();

	AddMovementInput(direction, AxisValue);
}

void ACPlayer::OnVerticalLook(float AxisValue)
{	
	AddControllerPitchInput(AxisValue);
}

void ACPlayer::OnHorizontalLook(float AxisValue)
{	
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

void ACPlayer::OnParkour()
{
	CheckTrue(IsMontagePlaying);
	CheckTrue(GetCharacterMovement()->IsFalling());
	CheckTrue(bParkouring);

	if (ParkourDatas[0].Montage)
	{
		bParkouring = true;

		ParkourTargetRot = GetLastMovementInputVector().Rotation();

		ParkourDatas[0].PlayMontage(this);

		//CharacterComponent->SetIsMontagePlaying(true);
		IsMontagePlaying = true;
	}

	//bParkouring = true;
}

void ACPlayer::OffParkour()
{
	bParkouring = false;
}

void ACPlayer::OnOnehand()
{
	CheckTrue(IsMontagePlaying);
	//CharacterComponent->SetIsMontagePlaying(true);
	IsMontagePlaying = true;

	IsPressedOnOnehand = true;

	if (CharacterComponent->GetIsWeaponOnehandMode())
	{
		bUnequipping = true;

		CharacterComponent->GetUnequipData(EWeaponType::Onehand).PlayMontage(this);

		GLog->Log("ACPlayer::OnOnehand() Unequip");
		
		return;
	}

	if(CharacterComponent->GetIsWeaponSpellMode())
	{
		bChanging = true;
		
		CharacterComponent->GetUnequipData(EWeaponType::Spell).PlayMontage(this);

		GLog->Log("ACPlayer::OnOnehand() Change");

		return;
	}

	if (CharacterComponent->GetIsWeaponSpellFistMode())
	{
		bChanging = true;

		SpellFistUnequipData[0].PlayMontage(this);

		GLog->Log("ACPlayer::OnOnehand() Change");

		return;
	}

	CharacterComponent->SetCurrentWeaponType(EWeaponType::Onehand);
	CharacterComponent->GetEquipData(EWeaponType::Onehand).PlayMontage(this);

	GLog->Log("ACPlayer::OnOnehand() Equip");
}

void ACPlayer::OnSpell()
{
	CheckTrue(IsMontagePlaying);
	//CharacterComponent->SetIsMontagePlaying(true);
	IsMontagePlaying = true;
	IsPressedOnSpell = true;

	if (CharacterComponent->GetIsWeaponOnehandMode())
	{
		bChanging = true;

		CharacterComponent->GetUnequipData(EWeaponType::Onehand).PlayMontage(this);

		GLog->Log("ACPlayer::OnSpell() Change");

		return;
	}

	if (CharacterComponent->GetIsWeaponSpellMode())
	{
		bUnequipping = true;

		CharacterComponent->GetUnequipData(EWeaponType::Spell).PlayMontage(this);

		GLog->Log("ACPlayer::OnSpell() Unequip");

		return;
	}

	if (CharacterComponent->GetIsWeaponSpellFistMode())
	{
		bChanging = true;

		SpellFistUnequipData[0].PlayMontage(this);

		GLog->Log("ACPlayer::OnOnehand() Change");

		return;
	}

	CharacterComponent->SetCurrentWeaponType(EWeaponType::Spell);
	CharacterComponent->GetEquipData(EWeaponType::Spell).PlayMontage(this);

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
	//CharacterComponent->SetIsMontagePlaying(true);
	IsMontagePlaying = true;
	IsPressedOnSpellFist = true;

	if (CharacterComponent->GetIsWeaponOnehandMode())
	{
		bChanging = true;

		CharacterComponent->GetUnequipData(EWeaponType::Onehand).PlayMontage(this);

		GLog->Log("ACPlayer::OnSpell() Change");

		return;
	}

	if (CharacterComponent->GetIsWeaponSpellMode())
	{
		bChanging = true;

		CharacterComponent->GetUnequipData(EWeaponType::Spell).PlayMontage(this);

		GLog->Log("ACPlayer::OnSpell() Unequip");

		return;
	}

	if (CharacterComponent->GetIsWeaponSpellFistMode())
	{
		bUnequipping = true;

		SpellFistUnequipData[0].PlayMontage(this);

		GLog->Log("ACPlayer::OnOnehand() Change");

		return;
	}

	CharacterComponent->SetCurrentWeaponType(EWeaponType::SpellFist);
	SpellFistEquipData[0].PlayMontage(this);

	GLog->Log("ACPlayer::OnSpell() Equip");
}

void ACPlayer::OnShield()
{
	CheckFalse(CharacterComponent->GetIsWeaponOnehandMode());

	ActivateShieldEquipEffect();
	CharacterComponent->SetCurrentWeaponType(EWeaponType::Shield);
	
	
	//// Shield 장착 에님 몽타주
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
	// MEMO: 이 Setter는 Normal Attack과 Skill Attack 둘 다 사용 (취소) Enemy Strafing 구현하면 좀 더 자연스러워 질 듯함
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
				WarriorNormalAttackBossDatas[Index].PlayMontage(this);

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
			/* BUG: 클래스 데이터 멤버 불 변수로 바꾸니까 잘 작동을 한다.
				 * 아마도 시간 복잡도? 뭐 그런 문제 때문에 이 부분에 들어오면 안됬는데 들어와버렸던 것 같다.
				 * 1번 째 공격도 가끔씩 클릭 할 때마다 다시 시작하는 것으로 인해 확신하여 수정하였다. */
			bAttacking = true;
			SpellFistDatas[Index].PlayMontage(this);
			GLog->Log("ACPlayer::OnAction() SpellFistAttack");
		}
	}
}

void ACPlayer::OnSkillOne()
{
	CheckTrue(IsMontagePlaying);
	CheckFalse(CanOnSkill1);
	
	SetPlayerActivateSkill(true);

	IsMontagePlaying = true;
	
	CharacterComponent->SetMp(-500.0f);

	if (CharacterComponent->GetIsWeaponOnehandMode())
	{	
		CharacterComponent->SetCurrentStateType(EStateType::Attack);


		if (CharacterComponent->GetCriticalDatasOnehand(0).Montage)
		{
			CharacterComponent->GetCriticalDatasOnehand(0).PlayMontage(this);
			ActivateCoolTimeSkillOne();
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
			ActivateCoolTimeSkillOne();
			IsMontagePlaying = true;
		}
	}
}

void ACPlayer::OffSkillOne()
{
	if (CharacterComponent->GetIsWeaponOnehandMode())
	{
		//if (CharacterComponent->GetCriticalDatasOnehand(1).Montage)
		//	CharacterComponent->GetCriticalDatasOnehand(1).PlayMontage(this);

		//GetWorldTimerManager().ClearTimer(WarriorSkillTimer);
	}
	else if (CharacterComponent->GetIsWeaponSpellMode())
	{
		if (Crosshair_SpellMeteor)
			Crosshair_SpellMeteor->DestroyCrosshair();
		
		WidgetComponent->SetbCrosshairVisible(true);

		//CharacterComponent->SetIsMontagePlaying(true);
		IsMontagePlaying = true;
		CharacterComponent->SetCurrentStateType(EStateType::Attack);

		CharacterComponent->GetCriticalDatasSpell(0).PlayMontage(this);

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
	CheckTrue(IsMontagePlaying);
	CheckFalse(CanOnSkill2);

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
			ActivateCoolTimeSkillTwo();
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
			ActivateCoolTimeSkillTwo();
		}
	}
	else if (CharacterComponent->GetIsWeaponSpellFistMode())
	{
		CharacterComponent->SetCurrentStateType(EStateType::Attack);
		IsMontagePlaying = true;

		if (SpellFistSkillDatas[1].Montage)
		{
			SpellFistSkillDatas[1].PlayMontage(this);
			ActivateCoolTimeSkillTwo();
		}
	}
}

void ACPlayer::OnSkillThree()
{
	CheckTrue(IsMontagePlaying);
	CheckFalse(CanOnSkill3);
	
	SetPlayerActivateSkill(true);

	CharacterComponent->SetMp(-500.0f);
	
	if (CharacterComponent->GetIsWeaponOnehandMode())
	{

		CharacterComponent->SetCurrentStateType(EStateType::Attack);
		//CharacterComponent->SetIsMontagePlaying(true);
		IsMontagePlaying = true;

		if (CharacterComponent->GetCriticalDatasOnehand(2).Montage)
		{
			//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
			CharacterComponent->GetCriticalDatasOnehand(2).PlayMontage(this);
			ActivateCoolTimeSkillThree();
		}
	}
	else if (CharacterComponent->GetIsWeaponSpellMode())
	{
		IsMontagePlaying = true;
		
		if (CharacterComponent->GetCriticalDatasSpell(2).Montage)
		{
			CharacterComponent->GetCriticalDatasSpell(2).PlayMontage(this);
			ActivateCoolTimeSkillThree();
		}
	}
	else if (CharacterComponent->GetIsWeaponSpellFistMode())
	{
		IsMontagePlaying = true;
		
		if (SpellFistSkillDatas[2].Montage)
		{
			SpellFistSkillDatas[2].PlayMontage(this);
			ActivateCoolTimeSkillThree();
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

	// 임시
	//FTimerHandle timer;

	//GetWorldTimerManager().SetTimer(timer, this, &UCCharacterComponent::SetSp(100.0f), )
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
 
















//void ACPlayer::TakeDamage_Mission2Cannon(float InDamage)
//{
//	CharacterComponent->SetHp((int32)-InDamage);
//}

void ACPlayer::Debug_OnControllerRotationYaw()

{
	if (bUseControllerRotationYaw)
		bUseControllerRotationYaw = false;
	else
		bUseControllerRotationYaw = true;
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


//void ACPlayer::ActivateCoolTimeSkillOne_Implementation()
//{
//	FTimerHandle timer;
//
//	GetWorld()->GetTimerManager().SetTimer(timer, FTimerDelegate::CreateLambda([&]() 
//	{
//		if (OnSkillCoolTimeCounting.IsBound())
//			OnSkillCoolTimeCounting.Broadcast();
//
//	}), 1.0f, true);
//}
//
//void ACPlayer::ActivateCoolTimeSkillTwo_Implementation()
//{
//	FTimerHandle timer;
//
//	GetWorld()->GetTimerManager().SetTimer(timer, FTimerDelegate::CreateLambda([&]()
//	{
//		if (OnSkillCoolTimeCounting.IsBound())
//			OnSkillCoolTimeCounting.Broadcast();
//
//	}), 1.0f, true);
//}
//
//void ACPlayer::ActivateCoolTimeSkillThree_Implementation()
//{
//	FTimerHandle timer;
//
//	GetWorld()->GetTimerManager().SetTimer(timer, FTimerDelegate::CreateLambda([&]()
//	{
//		if (OnSkillCoolTimeCounting.IsBound())
//			OnSkillCoolTimeCounting.Broadcast();
//
//	}), 1.0f, true);
//}
