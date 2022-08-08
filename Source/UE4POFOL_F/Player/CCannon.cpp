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
}

void ACCannon::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Action", EInputEvent::IE_Pressed, this, &ACCannon::OnFire);
	PlayerInputComponent->BindAction("Action", EInputEvent::IE_Released, this, &ACCannon::OffFire);
	PlayerInputComponent->BindAction("Unpossess", EInputEvent::IE_Pressed, this, &ACCannon::UnpossessCannon);
	PlayerInputComponent->BindAction("Critical", EInputEvent::IE_Pressed, this, &ACCannon::SetCanCriticalFireTrue);
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
	CannonStateTypeChange(ECannonStateType::Fire);
	
	ClickedOnFire = false;
	GetWorldTimerManager().ClearTimer(RangedTimer);
		
	if (RangedAvailCounter <= 3)
	{
		RangedAvailCounter = 0;

		if (CanOnCriticalFire)
		{
			GetWorldTimerManager().SetTimer(CriticalFinishTimer, this, &ACCannon::CriticalAvailChecker, 1.0f, true);
			GetWorldTimerManager().SetTimer(FireTimer, this, &ACCannon::FunctionBindForTimer_OnFireNormal, CriticalFireInterval, true);
			GetWorldTimerManager().SetTimer(HomingTimer, this, &ACCannon::OnFireHomingProjectile, CriticalHomingProjFireInterval, true);
			
			return;
		}

		ActivateNormalFireEffect();
		OnFireNormalProjectile();
	}
	else if (RangedAvailCounter > 3)
	{
		RangedAvailCounter = 0;

		ActivateCannonRangedAttackFireEffect();
		ShootRangedProjectile();
	}

	GLog->Log("ACCannon::OnFire()");
}

void ACCannon::FunctionBindForTimer_OnFireNormal()
{
	ActivateNormalFireEffect();
	OnFireNormalProjectile();
}

void ACCannon::SetCanCriticalFireTrue()
{
	ActivateFireCriticalEffect();
	
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
		RangedProjectile = ACProjectile::SpawnProjectile(this, CannonProjectileRangedClass, "Muzzle_01");

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
			RangedProjectile->ShootProjectile(UKismetMathLibrary::GetDirectionUnitVector(GetMesh()->GetSocketLocation("Muzzle_01"), WidgetComponent->GetHitResult().ImpactPoint));
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

	if (RangedAvailCounter == 4)
	{
		ActivateCannonRangedAttackReadyEffect();
		SpawnRangedProjectile();
	}
}

void ACCannon::CriticalAvailChecker()
{
	CriticalAvailCounter++;
}

void ACCannon::PossessCannon(class AActor* InOverlappedTriggerVolume, class AActor* InOtherActor)
{
	CheckNull(InOtherActor);

	ACPlayer* player = Cast<ACPlayer>(InOtherActor);

	Player = player;
	PlayerController = player->GetController();
	
	if (PlayerController)
	{
		
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
}