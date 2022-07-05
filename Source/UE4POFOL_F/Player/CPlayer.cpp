/* MEMO: CPlayer.cpp 코드 배치 정리 완료 
* .h 파일에 선언된 함수 원형 순으로 함수 정의를 작성하였다. */
//TODO: 코드 배치 상시 정리 확인
//TODO: OnAim(), OffAim() 보간 
//TODO: 점프 이슈 수정
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
#include "Weapon/CRifle.h"
#include "Weapon/CWeapon.h"
#include "Components/ArrowComponent.h"
#include "Enemy/CEnemy.h"
#include "Components/CapsuleComponent.h"
//#include "Weapon/CWeaponData.h"
#include "Weapon/CDoAction.h"
//#include "Skill/CSkill.h"
#include "Component/CWidgetComponent.h"
#include "Component/CCharacterComponent.h"
#include "Weapon/CProjectile.h"
#include "Object/CPortalCrosshair.h"
#include "Object/CPortalDoor.h"
#include "Object/CCrosshair.h"
#include "Object/CCrosshair_SpellThrow.h"

//#define LOG_PLAYER

ACPlayer::ACPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

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
		SpringArmComponent->TargetArmLength = 350.0f;
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
		//ParkourComponent = CreateDefaultSubobject<UCParkourComponent>("CPakourComponent");
		WidgetComponent = CreateDefaultSubobject<UCWidgetComponent>("CWidgetComponent");
		CharacterComponent = CreateDefaultSubobject<UCCharacterComponent>("CCharacterComponent");
	}

	{
		//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);//
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	}

	{
		//CHelpers::GetClass<UAnimInstance>(&AnimInstance, "AnimBlueprint'/Game/FORUE4POFOL/Player/ABP_CPlayer'");
		//GetMesh()->SetAnimInstanceClass(AnimInstance);
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

	ConstructorHelpers::FClassFinder<ACPortalCrosshair> assetCrosshair_Portal(TEXT("Blueprint'/Game/FORUE4POFOL/Player/Blueprint/BP_CPortalCrosshair'"));

	if (assetCrosshair_Portal.Succeeded())
		PortalCrosshairClass = assetCrosshair_Portal.Class;

	ConstructorHelpers::FClassFinder<ACCrosshair> assetCrosshair_SpellMeteor(TEXT("Blueprint'/Game/FORUE4POFOL/Player/Blueprint/BP_CWeapon_SpellMeteor'"));

	if (assetCrosshair_SpellMeteor.Succeeded())
		Crosshair_SpellMeteorClass = assetCrosshair_SpellMeteor.Class;

	ConstructorHelpers::FClassFinder<ACProjectile> assetPortalProjectile(TEXT("Blueprint'/Game/FORUE4POFOL/Player/Blueprint/BP_CProjectile_Portal'"));

	if (assetPortalProjectile.Succeeded())
		PortalProjectileClass = assetPortalProjectile.Class;

	ConstructorHelpers::FClassFinder<ACPortalDoor> assetPortalDoorEntrance(TEXT("Blueprint'/Game/FORUE4POFOL/Player/Blueprint/BP_CPortalDoor_Enterance'"));

	if (assetPortalDoorEntrance.Succeeded())
		PortalDoorEntranceClass = assetPortalDoorEntrance.Class;

	ConstructorHelpers::FClassFinder<ACPortalDoor> assetPortalDoorExit(TEXT("Blueprint'/Game/FORUE4POFOL/Player/Blueprint/BP_CPortalDoor_Exit'"));

	if (assetPortalDoorExit.Succeeded())
		PortalDoorExitClass = assetPortalDoorExit.Class;

	ConstructorHelpers::FClassFinder<ACWeapon> assetMeteorWeapon(TEXT("Blueprint'/Game/FORUE4POFOL/Player/Blueprint/BP_CWeapon_SpellMeteor'"));

	if (assetMeteorWeapon.Succeeded())
		SpellMeteorClass = assetMeteorWeapon.Class;

	//ConstructorHelpers::FClassFinder<ACProjectile> assetSpellThrowP(TEXT("Blueprint'/Game/FORUE4POFOL/Weapon/BP_CProjectile_SpellThrow'"));

	//if (assetSpellThrowP.Succeeded())
	//	SpellThrowProjectileClass = assetSpellThrowP.Class;
	
	RootComponent = GetCapsuleComponent();
}

void ACPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	//bUseControllerRotationYaw = false;
	//GetCharacterMovement()->bOrientRotationToMovement = true;

	Zooming = SpringArmComponent->TargetArmLength;

	GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &ACPlayer::MontageEnded);

	GetComponents<UCapsuleComponent>(CollisionCapsules);
	
	for (UShapeComponent* collision : CollisionCapsules)
	{
		collision->OnComponentBeginOverlap.AddDynamic(this, &ACPlayer::OnBeginOverlap);
		collision->OnComponentEndOverlap.AddDynamic(this, &ACPlayer::OnEndOverlap);
	}

	CharacterComponent->SetCurrentWeaponType(EWeaponType::Unarmed);
	//ParkourComponent->SetIdleMode();

	OnTimelineFloat.BindUFunction(this, "InZooming");
	Timeline.AddInterpFloat(AimCurve, OnTimelineFloat);
	Timeline.SetPlayRate(200);
}

void ACPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if(!GetCharacterMovement()->IsFalling())
		//GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	Timeline.TickTimeline(DeltaTime);

	if (bTargetting)
	{
		if (OutTargettingActorArr.Num() <= 0)
			return;

		if (OutTargettingActorArr[IndexTargetting])
		{
			//TargetLocation = OutTargettingActorArr[IndexTargetting]->GetActorLocation();
			GetController()->SetControlRotation(FMath::RInterpTo(GetActorRotation(), TargetRotator, DeltaTime, 2.0f));
		}
	}
	
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
	PlayerInputComponent->BindAction("Onehand", EInputEvent::IE_Pressed, this, &ACPlayer::OnOnehand);
	PlayerInputComponent->BindAction("Spell", EInputEvent::IE_Pressed, this, &ACPlayer::OnSpell);
	//PlayerInputComponent->BindAction("Rifle", EInputEvent::IE_Pressed, this, &ACPlayer::OnRifle);
	PlayerInputComponent->BindAction("Critical1", EInputEvent::IE_Pressed, this, &ACPlayer::OnCriticalOne);
	PlayerInputComponent->BindAction("Critical1", EInputEvent::IE_Released, this, &ACPlayer::OffCriticalOne);
	PlayerInputComponent->BindAction("Critical2", EInputEvent::IE_Pressed, this, &ACPlayer::OnCriticalTwo);
	PlayerInputComponent->BindAction("Skill", EInputEvent::IE_Pressed, this, &ACPlayer::OnSkill);
	PlayerInputComponent->BindAction("Action", EInputEvent::IE_Pressed, this, &ACPlayer::OnAction);
	PlayerInputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &ACPlayer::OnAim);
	PlayerInputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &ACPlayer::OffAim);
	//PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ACPlayer::OnFire);
	//PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &ACPlayer::OffFire);
	//PlayerInputComponent->BindAction("AutoFire", EInputEvent::IE_Pressed, this, &ACPlayer::OnAutoFire);
	//PlayerInputComponent->BindAction("Skill", EInputEvent::IE_Pressed, this, &ACPlayer::OnSkill);
	//PlayerInputComponent->BindAction("Critical", EInputEvent::IE_Pressed, this, &ACPlayer::OnCritical);
	PlayerInputComponent->BindAction("Parkour", EInputEvent::IE_Pressed, this, &ACPlayer::OnParkour);
	//PlayerInputComponent->BindAction("Block", EInputEvent::IE_Pressed, this, &ACPlayer::OnShield);
	//PlayerInputComponent->BindAction("Block", EInputEvent::IE_Released, this, &ACPlayer::OffShield);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACPlayer::OnJump);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ACPlayer::OffJump);
	//PlayerInputComponent->BindAction("Dash", EInputEvent::IE_DoubleClick, this, &ACPlayer::OnDash);
	//PlayerInputComponent->BindAction("ControllerRotationYaw", EInputEvent::IE_Pressed, this, &ACPlayer::OnControllerRotationYaw_Debug);
	
	//PlayerInputComponent->BindAction("Targetting_Select_Left", EInputEvent::IE_Pressed, this, &ACPlayer::TargettingSelectLeft);
	//PlayerInputComponent->BindAction("Targetting_Select_Right", EInputEvent::IE_Pressed, this, &ACPlayer::TargettingSelectRight);
}
 
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
	CheckFalse(CharacterComponent->GetbCanMove());
	CheckTrue(bAttacking);

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
	CheckTrue(bAttacking);

	CharacterComponent->SetCurrentStateType(EStateType::Move);
	
	FRotator rotation = FRotator(0, GetControlRotation().Yaw, 0);
	FVector direction = FQuat(rotation).GetRightVector().GetSafeNormal2D();

	AddMovementInput(direction, AxisValue);
}

void ACPlayer::OnVerticalLook(float AxisValue)
{	
	CheckTrue(bTargetting);
	
	AddControllerPitchInput(AxisValue);
}

void ACPlayer::OnHorizontalLook(float AxisValue)
{	
	CheckTrue(bTargetting);
	
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

void ACPlayer::OnJump()
{
	CheckFalse(CharacterComponent->GetbCanMove());
	CheckTrue(bAttacking);
	
	CharacterComponent->SetCurrentStateType(EStateType::Move);

	Jump();
}

void ACPlayer::OffJump()
{
	CharacterComponent->SetCurrentStateType(EStateType::Idle);

	StopJumping();
}

void ACPlayer::OnAim()
{	
	bAiming = true;
	
	if (CharacterComponent->GetIsWeaponOnehandMode())
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACEnemy::StaticClass(), OutTargettingActorArr);
		
		bTargetting = true;

		//bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;

		SpringArmComponent->TargetArmLength = 95.0f;
		SpringArmComponent->SocketOffset = FVector(0, 30, 10);
		SpringArmComponent->bEnableCameraLag = false;

		Timeline.PlayFromStart();
	}

	else if (CharacterComponent->GetIsWeaponSpellMode())
	{
		PlaySpellPortalOnAimSound();

		WidgetComponent->SetbCrosshairVisible(false);

		PortalCrosshair = ACPortalCrosshair::SpawnPortalCrosshair(this, PortalCrosshairClass);
		PortalCrosshair->SetOwner(this);
		PortalCrosshair->SetActorRotation(CameraComponent->GetComponentRotation());

		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		
		SpringArmComponent->TargetArmLength = 95.0f;
		SpringArmComponent->SocketOffset = FVector(0, 30, 10);
		SpringArmComponent->bEnableCameraLag = false;

		Timeline.PlayFromStart();
	}
}

void ACPlayer::OffAim()
{
	if (CharacterComponent->GetIsWeaponOnehandMode())
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;

		Timeline.ReverseFromEnd();

		SpringArmComponent->TargetArmLength = 350.0f;
		SpringArmComponent->SocketOffset = FVector(0, 60, 0);
		SpringArmComponent->bEnableCameraLag = true;

		bTargetting = false;
	}

	if (CharacterComponent->GetIsWeaponSpellMode())
	{
		if(PortalCrosshair)
			PortalCrosshair->DestroyPortalCrosshair();

		WidgetComponent->SetbCrosshairVisible(true);

		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;

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
	GetCharacterMovement()->MaxWalkSpeed = 600.0;
	IsRunning = true;
}

void ACPlayer::OffRun()
{
	GetCharacterMovement()->MaxWalkSpeed = 350.0;
	IsRunning = false;
}

void ACPlayer::OnDash()
{	
	//CheckTrue(bDashing);
	//
	//StateComponent->SetParkourMode();
	//
	//if (ParkourComponent->IsJumpMode())
	//{	
	//	//CheckFalse(//WeaponComponent->IsOnehandMode());
	//	
	//	LaunchCharacter(GetActorForwardVector() * 10000.0f, true, true);
	//	//WeaponComponent->GetWeaponData(0)->GetDoActionData(3).PlayMontage(this);
	//	
	//	//FLatentActionInfo latentInfo;
	//	//latentInfo.CallbackTarget = this;
	//	//
	//	//UKismetSystemLibrary::MoveComponentTo
	//	//(
	//	//	GetCapsuleComponent(),
	//	//	GetMesh()->GetSocketLocation(FName("jumpDashLocation")),
	//	//	GetActorRotation(),
	//	//	true,
	//	//	false,
	//	//	JumpDashEaseOutTime,
	//	//	false,
	//	//	EMoveComponentAction::Move,
	//	//	latentInfo
	//	//);
	//	
	//
	//	bDashing = true;
	//
	//	return;
	//}
	//
	//bDashing = true;
	//
	////WeaponComponent->GetWeaponData(0)->GetDoActionData(4).PlayMontage(this);
	//
	//GetCharacterMovement()->MaxWalkSpeed = DashSpeed;
	//GetCharacterMovement()->MaxAcceleration = 4096.0f;
}

void ACPlayer::OffDash()
{
	//CheckFalse(bDashing);
	//
	//GetCharacterMovement()->MaxWalkSpeed = 800.0f;
	//GetCharacterMovement()->MaxAcceleration = 2048.0f;
}

void ACPlayer::OnParkour()
{
	//CheckTrue(//WeaponComponent->IsRifleMode());
	//
	//ParkourComponent->DoParkour();
}

void ACPlayer::OnAction()
{
	CheckTrue(CharacterComponent->GetIsWeaponUnarmedMode());
	CheckTrue(bAiming);

	if (CharacterComponent->GetIsWeaponOnehandMode())
	{
		if (GetMovementComponent()->IsFalling())
		{
			//bAttacking = true;

			CharacterComponent->GetActionDatasOnehand(3).PlayMontage(this);

			return;
		}
		
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
			//CharacterComponent->SetIsMontagePlaying(true);
			//CharacterComponent->SetCurrentStateType(EStateType::Attack);
			/* BUG: 클래스 데이터 멤버 불 변수로 바꾸니까 잘 작동을 한다. 
			 * 아마도 시간 복잡도? 뭐 그런 문제 때문에 이 부분에 들어오면 안됬는데 들어와버렸던 것 같다.
			 * 1번 째 공격도 가끔씩 클릭 할 때마다 다시 시작하는 것으로 인해 확신하여 수정하였다. */
			bAttacking = true;
			CharacterComponent->GetActionDatasOnehand(Index).PlayMontage(this);
			GLog->Log("ACPlayer::OnAction() OnehandAttack");
		}

	}	
	else if (CharacterComponent->GetIsWeaponSpellMode())
	{
		CharacterComponent->SetIsMontagePlaying(true);
		//CharacterComponent->SetCurrentStateType(EStateType::Attack);		
		CharacterComponent->GetActionDatasSpell((int32)FMath::RandRange(0, 1)).PlayMontage(this);

		GLog->Log("ACPlayer::OnAction() SpellAttack");
	}
}

void ACPlayer::OnCriticalOne()
{
	if (CharacterComponent->GetIsWeaponOnehandMode())
	{
		CharacterComponent->SetCurrentStateType(EStateType::Attack);

		if (CharacterComponent->GetCriticalDatasOnehand(0).Montage)
			CharacterComponent->GetCriticalDatasOnehand(0).PlayMontage(this);

		CharacterComponent->SetIsMontagePlaying(true);
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
}

void ACPlayer::OffCriticalOne()
{
	if (CharacterComponent->GetIsWeaponOnehandMode())
	{
		if (CharacterComponent->GetCriticalDatasOnehand(1).Montage)
			CharacterComponent->GetCriticalDatasOnehand(1).PlayMontage(this);
	}
	else if (CharacterComponent->GetIsWeaponSpellMode())
	{
		if (Crosshair_SpellMeteor)
			Crosshair_SpellMeteor->DestroyCrosshair();
		
		WidgetComponent->SetbCrosshairVisible(true);

		CharacterComponent->SetIsMontagePlaying(true);
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

void ACPlayer::OnCriticalTwo()
{
	if (CharacterComponent->GetIsWeaponOnehandMode())
	{
	}
	else if (CharacterComponent->GetIsWeaponSpellMode())
	{
		CharacterComponent->SetCurrentStateType(EStateType::Attack);
		CharacterComponent->SetIsMontagePlaying(true);

		CharacterComponent->GetCriticalDatasSpell(1).PlayMontage(this);
	}

}

void ACPlayer::OnSkill()
{
	if (CharacterComponent->GetIsWeaponOnehandMode())
	{
	}
	else if (CharacterComponent->GetIsWeaponSpellMode())
	{
	}
}

void ACPlayer::ShootSpawnedProjectile(ACProjectile* InTargetProjectile, FName InShootSocketName)
{
	if (WidgetComponent->GetHitResult().GetActor())
		InTargetProjectile->ShootProjectile(UKismetMathLibrary::GetDirectionUnitVector(GetMesh()->GetSocketLocation(InShootSocketName), WidgetComponent->GetHitResult().ImpactPoint));
	else
		InTargetProjectile->ShootProjectile(CameraComponent->GetForwardVector());
}

void ACPlayer::SpawnSpellProjectileL()
{
	SpellProjectileL = ACProjectile::SpawnProjectile(this, CharacterComponent->GetProjectileClass(), FName("Spawn_Spell_Projectile_L"));
	SpellProjectileL->SetOwner(this);

	SpellProjectileL->SetActorRotation(CameraComponent->GetComponentRotation());

	ShootSpawnedProjectile(SpellProjectileL, "Spawn_Spell_Projectile_L");	
}

void ACPlayer::SpawnSpellProjectileR()
{
	SpellProjectileR = ACProjectile::SpawnProjectile(this, CharacterComponent->GetProjectileClass(), FName("Spawn_Spell_Projectile_R"));
	SpellProjectileR->SetOwner(this);

	SpellProjectileR->SetActorRotation(CameraComponent->GetComponentRotation());
	
	ShootSpawnedProjectile(SpellProjectileR, "Spawn_Spell_Projectile_R");
}

void ACPlayer::SpawnPortalProjectile()
{
	PortalProjectile = ACProjectile::SpawnProjectile(this, PortalProjectileClass, FName("Spawn_Spell_Projectile_R"));
	PortalProjectile->SetOwner(this);
	
	PortalProjectile->SetActorRotation(CameraComponent->GetComponentRotation());
	
	ShootSpawnedProjectile(PortalProjectile, "Spawn_Spell_Projectile_R");
}

void ACPlayer::SpawnSpellThrowProjectile()
{
	SpellThrowProjectile = ACProjectile::SpawnProjectile(this, SpellThrowProjectileClass, FName("Spawn_SpellThrow_Projectile"));
	SpellThrowProjectile->SetOwner(this);

	SpellThrowProjectile->SetActorRotation(CameraComponent->GetComponentRotation());

	ShootSpawnedProjectile(SpellThrowProjectile, "Spawn_SpellThrow_Projectile");
}

void ACPlayer::SpawnSpellMeteorWeapon()
{
	SpellMeteorWeapon = ACWeapon::SpawnWeapon(this, SpellMeteorClass, Cast<ACCrosshair_SpellThrow>(Crosshair_SpellMeteor)->GetImpactPoint());
	SpellMeteorWeapon->SetOwner(this);
}

void ACPlayer::SpawnPortalDoorEntrance()
{
	PortalDoorEntrance = ACPortalDoor::SpawnPortalDoor(this, GetMesh()->GetSocketLocation("Spawn_Spell_Projectile_R"), PortalDoorEntranceClass);
}

void ACPlayer::SpawnPortalDoorExit()
{
	PortalDoorExit = ACPortalDoor::SpawnPortalDoor(this, GetMesh()->GetSocketLocation("FX_Ring_Outer"), PortalDoorExitClass);
}

void ACPlayer::SetPlayerPortalLocation()
{
	bPortalTeleporting = true;
	
	FVector portalLocation = PortalCrosshair->GetHitResult().ImpactPoint;
	
	CurrentCameraEffectType = ECameraEffectType::Teleport;

	SpawnCameraEffect();

	SetActorLocation(FVector(portalLocation.X + 20.0f, portalLocation.Y + 20.0f, portalLocation.Z + 150.0f));
}

void ACPlayer::BeginNextAction()
{
	if (bCanNextAction)
	{
		bCanNextAction = false;

		Index++;

		CharacterComponent->GetActionDatasOnehand(Index).PlayMontage(this);
	}
}

void ACPlayer::EndThisAction()
{
	Index = 0;
	bCanCombo = false;
	bCanNextAction = false;
	bAttacking = false;
}

void ACPlayer::OnOnehand()
{
	CheckTrue(CharacterComponent->GetIsMontagePlaying());
	CharacterComponent->SetIsMontagePlaying(true);

	if(CharacterComponent->GetIsWeaponSpellMode())
	{
		bChanging = true;
		
		CharacterComponent->GetUnequipData(CharacterComponent->GetCurrentWeaponType()).PlayMontage(this);

		GLog->Log("ACPlayer::OnOnehand() Change");

		return;
	}

	if (CharacterComponent->GetIsWeaponOnehandMode())
	{
		bUnequipping = true;

		CharacterComponent->GetUnequipData(EWeaponType::Onehand).PlayMontage(this);

		GLog->Log("ACPlayer::OnOnehand() Unequip");
		
		return;
	}

	CharacterComponent->SetCurrentWeaponType(EWeaponType::Onehand);
	CharacterComponent->GetEquipData(EWeaponType::Onehand).PlayMontage(this);

	GLog->Log("ACPlayer::OnOnehand() Equip");
}

void ACPlayer::OnSpell()
{
	CheckTrue(CharacterComponent->GetIsMontagePlaying());
	CharacterComponent->SetIsMontagePlaying(true);

	if (CharacterComponent->GetIsWeaponOnehandMode())
	{
		bChanging = true;

		CharacterComponent->GetUnequipData(CharacterComponent->GetCurrentWeaponType()).PlayMontage(this);

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

	CharacterComponent->SetCurrentWeaponType(EWeaponType::Spell);
	CharacterComponent->GetEquipData(EWeaponType::Spell).PlayMontage(this);

	GLog->Log("ACPlayer::OnSpell() Equip");
}

void ACPlayer::OnOnehandSpell()
{
	
}

void ACPlayer::OnShield()
{
	//SpringArmComponent->TargetArmLength = 250.0f;
	//
	//GLog->Log("On Shield");
}

void ACPlayer::ShieldDefencing()
{
	////PlayAnimMontage(BlockAnimMontage, 0.8f);
	//
	//if (OnPlayerActiveBlock.IsBound())
	//{
	//	OnPlayerActiveBlock.Broadcast(true);
	//	
	//	LaunchCharacter(GetActorForwardVector() * -1000.0f, false, false);
	//
	//	GLog->Log("My(Player) ShieldDefencing Successed!");
	//}
}

void ACPlayer::OffShield()
{
	SpringArmComponent->TargetArmLength = 350.0f;

	GLog->Log("Off Shield");
}

float ACPlayer::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	CharacterComponent->SetCurrentStateType(EStateType::Damage);
	
	Damaged.DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser); // damage float
	//Damaged.DamageEvent = (FActionDamageEvent*)&DamageEvent;  // Hit 에니메이션 몽타주
	Damaged.EventInstigator = EventInstigator; // Player 이므로 Enemy controller
	Damaged.DamageCauser = DamageCauser; // weapon
	Damaged.DamageAmount = DamageAmount;
	
	CharacterComponent->SetHp(-Damaged.DamageAmount);

	CurrentCameraEffectType = ECameraEffectType::Damage;
	//CharacterComponent->SetIsMontagePlaying(true);

	SpawnCameraEffect();
	ShakeCamera();
	
	if (CharacterComponent->GetCurrentHp() <= 0.0f)
	{
		// TODO: Dead Montage BP에서 아직 할당 안했음 할 것
		//CharacterComponent->GetDamageData(1).PlayMontage(this);

		GLog->Log("ACPlayer::TakeDamage() And Dead");

		return Damaged.DamageAmount;
	}
	
	//CharacterComponent->GetDamageData(0).PlayMontage(this);

	GLog->Log("ACPlayer::TakeDamage()");

	return Damaged.DamageAmount;
}

void ACPlayer::OnCollision()
{
	for (UShapeComponent* collision : CollisionCapsules)
		collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ACPlayer::OffCollision()
{
	for (UShapeComponent* collision : CollisionCapsules)
		collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACPlayer::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CheckTrue(OverlappedComponent == GetCapsuleComponent());

}

void ACPlayer::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CheckTrue(OverlappedComponent == GetCapsuleComponent());

}

void ACPlayer::MontageEnded(UAnimMontage* InMontage, bool Ininterrupted)
{
	if (bUnequipping)
	{
		CharacterComponent->SetCurrentWeaponType(EWeaponType::Unarmed);

		bUnequipping = false;
		CharacterComponent->SetIsMontagePlaying(false);

		return;
	}

	if (bChanging)
	{
		if (CharacterComponent->GetIsWeaponOnehandMode())
		{
			CharacterComponent->SetCurrentWeaponType(EWeaponType::Spell);

			CharacterComponent->GetEquipData(EWeaponType::Spell).PlayMontage(this);

			bChanging = false;
			CharacterComponent->SetIsMontagePlaying(false);

			return;
		}
		else if (CharacterComponent->GetIsWeaponSpellMode())
		{
			CharacterComponent->SetCurrentWeaponType(EWeaponType::Onehand);

			CharacterComponent->GetEquipData(EWeaponType::Onehand).PlayMontage(this);

			bChanging = false;
			CharacterComponent->SetIsMontagePlaying(false);

			return;
		}
	}

	if (CharacterComponent->GetCurrentStateType() == EStateType::Attack)
		CharacterComponent->SetCurrentStateType(EStateType::Idle);

	if (bPortalTeleporting)
		bPortalTeleporting = false;

	if (!CharacterComponent->GetbCanMove())
		CharacterComponent->SetbCanMove(true);
	if (!CharacterComponent->GetbFixedCamera())
		CharacterComponent->SetbFixedCamera(true);

	//if (bAttacking)
		//bAttacking = false;
	//bCanCombo = false;
	//Index = 0;

	CharacterComponent->SetIsMontagePlaying(false);
}

void ACPlayer::SpawnEmitter()
{
	if (OnehandSkillEffect1Class)
	{
		// CheckFalse(IsOnehandSkillMode);

		FTransform targetTransform = GetMesh()->GetSocketTransform(FName("spawnSkillEffect1"));

		ACWeapon* spawnedWeapon = GetWorld()->SpawnActorDeferred<ACWeapon>(OnehandSkillEffect1Class, targetTransform, this);

		spawnedWeapon->SetOwner(this);

		//spawnedWeapon->OnWeaponBeginOverlapForSkill.AddDynamic(//WeaponComponent->GetWeaponData(0)->GetSkill(), &UCSkill::OnWeaponBeginOverlap);
		//spawnedWeapon->OnWeaponEndOverlapForSkill.AddDynamic(//WeaponComponent->GetWeaponData(0)->GetSkill(), &UCSkill::OnWeaponEndOverlap);

		UGameplayStatics::FinishSpawningActor(spawnedWeapon, targetTransform);
	}
}

void ACPlayer::SpawnEmitter2()
{
	if (OnehandSkillEffect2Class)
	{
		// CheckFalse(IsOnehandSkillMode);

		FTransform targetTransform = GetMesh()->GetSocketTransform(FName("spawnSkillEffect2"));

		ACWeapon* spawnedWeapon = GetWorld()->SpawnActorDeferred<ACWeapon>(OnehandSkillEffect2Class, targetTransform, this);

		spawnedWeapon->SetOwner(this);

		//spawnedWeapon->OnWeaponBeginOverlapForSkill.AddDynamic(//WeaponComponent->GetWeaponData(0)->GetSkill(), &UCSkill::OnWeaponBeginOverlap);
		//spawnedWeapon->OnWeaponEndOverlapForSkill.AddDynamic(//WeaponComponent->GetWeaponData(0)->GetSkill(), &UCSkill::OnWeaponEndOverlap);

		UGameplayStatics::FinishSpawningActor(spawnedWeapon, targetTransform);
	}
}

void ACPlayer::OnControllerRotationYaw_Debug()
{
	if (bUseControllerRotationYaw)
		bUseControllerRotationYaw = false;
	else
		bUseControllerRotationYaw = true;
}

void ACPlayer::PlayerLog_Debug()
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

FVector ACPlayer::CalculateMeshSocketToVectorLocation(FName InSocketName, FVector InDirectionTo)
{
	return UKismetMathLibrary::GetDirectionUnitVector(GetMesh()->GetSocketLocation(InSocketName), InDirectionTo);
}

void ACPlayer::ShakeCamera()
{
	if (GetWorld()->GetFirstPlayerController() && DamageCameraShakeClass)
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->StartCameraShake(DamageCameraShakeClass);
}