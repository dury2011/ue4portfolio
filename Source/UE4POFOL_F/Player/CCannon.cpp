#include "Player/CCannon.h"
#include "Global.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Weapon/CProjectile.h"
#include "Component/CWidgetComponent.h"
#include "Object/CTriggerVolume_Cannon.h"
#include "Player/CPlayer.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Component/CCharacterComponent.h"

// public: //////////////////////////////////////////////////////////////////////
ACCannon::ACCannon()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = GetCapsuleComponent();

	ConstructorHelpers::FObjectFinder<USkeletalMesh> assetSkeletalMesh(*FString("SkeletalMesh'/Game/ParagonProps/Turrets/Turrets_inhibitors/Meshes/Turret_Inhib_Light_GDC'"));

	if (assetSkeletalMesh.Succeeded())
		SkeletalMesh = assetSkeletalMesh.Object;

	//ConstructorHelpers::FClassFinder<ACProjectile> classProjectile(*FString("Blueprint'/Game/FORUE4POFOL/Player/Blueprint/BP_CCannon.BP_CCannon_C'"));

	//if (classProjectile.Succeeded())
		//CannonProjectileClass = classProjectile.Class;

	WidgetComponent = CreateDefaultSubobject<UCWidgetComponent>("CWidgetComponent");
	GetMesh()->SetSkeletalMesh(SkeletalMesh);
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	SpringArm->SetupAttachment(GetMesh());
	Camera->SetupAttachment(SpringArm);
	SpringArm->SetRelativeLocation(FVector(-550.0f, 300.0f, 750.0f));
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));

	GetComponents<UCapsuleComponent>(CapsuleCollisions);

	for (UShapeComponent* collision : CapsuleCollisions)
	{
		collision->OnComponentBeginOverlap.AddDynamic(this, &ACCannon::OnBeginOverlap);
		collision->OnComponentEndOverlap.AddDynamic(this, &ACCannon::OnEndOverlap);
		collision->OnComponentHit.AddDynamic(this, &ACCannon::OnHit);
	}

	CannonStateTypeChange(ECannonStateType::Unpossessed);
}

void ACCannon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Pitch = UKismetMathLibrary::NormalizedDeltaRotator(GetBaseAimRotation(), GetActorRotation()).Pitch;
	Roll = UKismetMathLibrary::NormalizedDeltaRotator(GetBaseAimRotation(), GetActorRotation()).Roll;
	Yaw = UKismetMathLibrary::NormalizedDeltaRotator(GetBaseAimRotation(), GetActorRotation()).Yaw;

	if (IsNowReloading)
	{
		ClickedOnFire = false;

		if(GetWorldTimerManager().IsTimerActive(RangedTimer))
			GetWorldTimerManager().ClearTimer(RangedTimer);
	}

	if (ClickedOnFire)
	{
		GetWorldTimerManager().SetTimer(RangedTimer, this, &ACCannon::RangedAvailChecker, 1.0f, true);

		ClickedOnFire = false;
	}

	if (CriticalAvailCounter == 8)
	{
		GetWorldTimerManager().ClearTimer(FireTimer);
		GetWorldTimerManager().ClearTimer(HomingTimer);
		GetWorldTimerManager().ClearTimer(CriticalFinishTimer);

		CanOnCriticalFire = false;
		CriticalAvailCounter = 0;
	}

	if (RangedProjectile && RangedAvailCounter > 0)
	{
		RangedProjectile->SetActorLocation(GetMesh()->GetSocketLocation("Muzzle_02"));
	}
}

void ACCannon::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Action", EInputEvent::IE_Pressed, this, &ACCannon::OnFire);
	PlayerInputComponent->BindAction("Action", EInputEvent::IE_Released, this, &ACCannon::OffFire);
	PlayerInputComponent->BindAction("Unpossess", EInputEvent::IE_Pressed, this, &ACCannon::UnpossessCannon);
	PlayerInputComponent->BindAction("Critical", EInputEvent::IE_Pressed, this, &ACCannon::SetCanCriticalFireTrue);
	PlayerInputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &ACCannon::OnReload);
	//PlayerInputComponent->BindAxis("OnCannonSpecial", this, &ACCannon::OnFire);
	PlayerInputComponent->BindAxis("VerticalLook", this, &ACCannon::OnVerticalLook);
	PlayerInputComponent->BindAxis("HorizontalLook", this, &ACCannon::OnHorizontalLook);
}

void ACCannon::OnFireNormalProjectile()
{
	if (CannonProjectileClass)
	{
		CannonProjectile = ACProjectile::SpawnProjectile(this, CannonProjectileClass, "Muzzle_01");

		if (CannonProjectile)
		{
			CannonProjectile->SetOwner(this);
			CannonProjectile->SetActorRotation(Camera->GetComponentRotation());

			if (WidgetComponent->GetHitResult().GetActor())
				CannonProjectile->ShootProjectile(UKismetMathLibrary::GetDirectionUnitVector(GetMesh()->GetSocketLocation("Muzzle_01"), WidgetComponent->GetHitResult().ImpactPoint));
			else
				CannonProjectile->ShootProjectile(Camera->GetForwardVector());
		}
	}
}

float ACCannon::GetHpPercentage()
{
	return CurrentHp / MaxHp * 100.0f;
}

// private: //////////////////////////////////////////////////////////////////////
void ACCannon::OnVerticalLook(float AxisValue)
{
	if (Pitch >= 45.0f || Pitch <= -0.1f)
		return;
	
	AddControllerPitchInput(AxisValue);
}

void ACCannon::OnHorizontalLook(float AxisValue)
{
	if (Yaw >= 90.0f || Yaw <= -90.0f)
		return;
	
	AddControllerYawInput(AxisValue);
}

void ACCannon::OnFire()
{
	ClickedOnFire = true;
}

void ACCannon::OffFire()
{
	CheckTrue(IsNowReloading);
	
	if (CurrentAmmo <= 0)
	{
		ActivateZeroAmmoSound();

		return;
	}
	
	CannonStateTypeChange(ECannonStateType::Fire);
	
	ClickedOnFire = false;
	GetWorldTimerManager().ClearTimer(RangedTimer);
		
	if (RangedAvailCounter <= (RangedAvailTime - 1.0f))
	{
		RangedAvailCounter = 0;
		SpringArm->SetRelativeLocation(FVector(-550.0f, 300.0f, 750.0f));

		if (CanOnCriticalFire)
		{
			GetWorldTimerManager().SetTimer(CriticalFinishTimer, this, &ACCannon::CriticalAvailChecker, 1.0f, true);
			GetWorldTimerManager().SetTimer(FireTimer, this, &ACCannon::FunctionBindForTimer_OnFireNormal, CriticalFireInterval, true);
			GetWorldTimerManager().SetTimer(HomingTimer, this, &ACCannon::OnFireHomingProjectile, CriticalHomingProjFireInterval, true);
			
			return;
		}

		CurrentAmmo--;

		if (OnCannonFire.IsBound())
			OnCannonFire.Broadcast(CurrentAmmo);

		ActivateNormalFireEffect();
		OnFireNormalProjectile();
	}
	else if (RangedAvailCounter > (RangedAvailTime - 1.0f))
	{
		RangedAvailCounter = 0;

		CurrentAmmo--;

		if (OnCannonFire.IsBound())
			OnCannonFire.Broadcast(CurrentAmmo);

		SpringArm->SetRelativeLocation(FVector(-550.0f, 300.0f, 750.0f));
		ActivateCannonRangedAttackFireEffect();
		ShootRangedProjectile();
	}

	GLog->Log("ACCannon::OnFire()");
}

void ACCannon::OnReload()
{
	IsNowReloading = true;
	
	GetWorldTimerManager().SetTimer(ReloadTimer, this, &ACCannon::ReloadChecker, 1.0f, true);

	if (OnCannonReloading.IsBound())
		OnCannonReloading.Broadcast(true);
}

void ACCannon::FunctionBindForTimer_OnFireNormal()
{
	ActivateNormalFireEffect();
	OnFireNormalProjectile();
}

void ACCannon::SetCanCriticalFireTrue()
{
	ActivateCriticalFireEffect();
	
	CanOnCriticalFire = true;
}

void ACCannon::OnFireHomingProjectile()
{
	if (HomingProjectileClass)
	{
		ACProjectile* homingProjectile = ACProjectile::SpawnProjectile(this, HomingProjectileClass, "SpawnHomingProjectile");

		if (homingProjectile)
		{
			homingProjectile->SetOwner(this);
			//homingProjectile->SetActorRotation(Camera->GetComponentRotation());
			homingProjectile->ShootProjectile(UKismetMathLibrary::GetDirectionUnitVector(GetMesh()->GetSocketLocation("SpawnHomingProjectile"), WidgetComponent->GetHitResult().ImpactPoint));
		}
	}
}

void ACCannon::SpawnRangedProjectile()
{
	if (CannonProjectileRangedClass)
	{
		RangedProjectile = ACProjectile::SpawnProjectile(this, CannonProjectileRangedClass, "Muzzle_02");

		if (RangedProjectile)
		{
			RangedProjectile->SetActorRotation(Camera->GetComponentRotation());
			RangedProjectile->ProjectileComponent->InitialSpeed = 0.0f;
			RangedProjectile->ProjectileComponent->MaxSpeed = 0.0f;
			RangedProjectile->SetOwner(this);
		}
	}
}

void ACCannon::ShootRangedProjectile()
{
	if (RangedProjectile)
	{
		RangedProjectile->ProjectileComponent->InitialSpeed = 5000.0f;
		RangedProjectile->ProjectileComponent->MaxSpeed = 5000.0f;
		
		if (WidgetComponent->GetHitResult().GetActor())
			RangedProjectile->ShootProjectile(UKismetMathLibrary::GetDirectionUnitVector(GetMesh()->GetSocketLocation("Muzzle_02"), WidgetComponent->GetHitResult().ImpactPoint));
	}		
}

void ACCannon::CannonStateTypeChange(ECannonStateType InType)
{
	PreviousCannonType = CurrentCannonType;
	CurrentCannonType = InType;

	if (OnCannonStateTypeChange.IsBound())
		OnCannonStateTypeChange.Broadcast(PreviousCannonType, CurrentCannonType);
}

void ACCannon::RangedAvailChecker()
{
	RangedAvailCounter++;

	if ((RangedAvailCounter == RangedAvailTime) && !IsNowReloading)
	{
		SpringArm->SetRelativeLocation(FVector(-500.0f, 300.0f, 540.0f));
		ActivateCannonRangedAttackReadyEffect();
		SpawnRangedProjectile();
	}
}

void ACCannon::CriticalAvailChecker()
{
	CriticalAvailCounter++;
}

// MEMO: keyinput으로 트리거되는 델리게이트 말고는 C++ 함수에서는 Broadcast가 안되는 듯?
void ACCannon::ReloadChecker()
{
	if (ReloadCounter == 1)
	{
		ReloadCounter = 0;
		
		IsNowReloading = false;
		
		GetWorldTimerManager().ClearTimer(ReloadTimer);

		CurrentAmmo = 50;

		return;
	}
	
	ReloadCounter += 1;
}

void ACCannon::PossessCannon(class AActor* InOverlappedTriggerVolume, class AActor* InOtherActor)
{
	CheckNull(InOtherActor);

	ACPlayer* player = Cast<ACPlayer>(InOtherActor);

	Player = player;

	//PlayerInformation.MaxHp = player->CharacterComponent->GetMaxHp();
	//PlayerInformation.CurrentHp = player->CharacterComponent->GetCurrentHp();
	//PlayerInformation.MaxMp = player->CharacterComponent->GetMaxMp();
	//PlayerInformation.CurrentMp = player->CharacterComponent->GetCurrentMp();

	PlayerController = player->GetController();
	
	if (PlayerController)
	{
		//Player->CharacterComponent->SetMaxHp(0.0f);
		//Player->CharacterComponent->SetHp(0.0f);
		//
		//Player->CharacterComponent->SetMaxHp(2000.0f);
		//Player->CharacterComponent->SetHp(2000.0f);
		
		PlayerController->UnPossess();
		PlayerController->Possess(this);
		
		Player->SetActorLocation(TriggerVolume->GetActorLocation());

		CannonStateTypeChange(ECannonStateType::Possessed);

		ActivatePossessCannonEffect();

		GLog->Log("ACCannon::PossessCannon()");
	}
	//if (player)
	//{
	//	player->UnPossessed();
	//	
	//	PossessedBy(player->GetController());
	//}
}

void ACCannon::UnpossessCannon()
{
	CheckFalse(CurrentCannonType == ECannonStateType::Possessed);
	
	if (PlayerController && Player)
	{
		PlayerController->UnPossess();
		PlayerController->Possess(Player);

		CannonStateTypeChange(ECannonStateType::Unpossessed);

		Player->SetActorLocation(TriggerVolume->GetActorLocation());

		//Player->CharacterComponent->SetHp(PlayerInformation.CurrentHp);

		DeactivatePossessCannonEffect();

		GLog->Log("ACCannon::UnpossessCannon()");
	}
}

void ACCannon::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{

}

void ACCannon::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ACCannon::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
}

float ACCannon::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (CurrentHp <= 0)
		return 0.0f;

	CurrentHp -= DamageAmount;

	//if (Player)
	//{
	//	Player->TakeDamage_Mission2Cannon(DamageAmount);
	//}

	return DamageAmount;
}

// protected: //////////////////////////////////////////////////////////////////////
void ACCannon::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> outActorArr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACTriggerVolume_Cannon::StaticClass(), outActorArr);

	for (int i = 0; i < outActorArr.Num(); i++)
		TriggerVolume = dynamic_cast<ACTriggerVolume_Cannon*>(outActorArr[i]);

	if (TriggerVolume)
		TriggerVolume->OnCannonEventTrigger.AddDynamic(this, &ACCannon::PossessCannon);

	if (OnCannonFire.IsBound())
		OnCannonFire.Broadcast(CurrentAmmo);
}