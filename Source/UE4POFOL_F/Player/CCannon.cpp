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

ACCannon::ACCannon()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = GetCapsuleComponent();

	ConstructorHelpers::FObjectFinder<USkeletalMesh> assetSkeletalMesh(*FString("SkeletalMesh'/Game/ParagonProps/Turrets/Turrets_inhibitors/Meshes/Turret_Inhib_Light_GDC'"));

	if (assetSkeletalMesh.Succeeded())
		SkeletalMesh = assetSkeletalMesh.Object;

	ConstructorHelpers::FClassFinder<ACProjectile> classProjectile(*FString("Blueprint'/Game/FORUE4POFOL/Weapon/BP_CProjectile_Cannon.BP_CProjectile_Cannon_C'"));

	if (classProjectile.Succeeded())
		CannonProjectileClass = classProjectile.Class;

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

void ACCannon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Pitch = UKismetMathLibrary::NormalizedDeltaRotator(GetBaseAimRotation(), GetActorRotation()).Pitch;
	Roll = UKismetMathLibrary::NormalizedDeltaRotator(GetBaseAimRotation(), GetActorRotation()).Roll;
	Yaw = UKismetMathLibrary::NormalizedDeltaRotator(GetBaseAimRotation(), GetActorRotation()).Yaw;
}

void ACCannon::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Action", EInputEvent::IE_Pressed, this, &ACCannon::OnFire);
	PlayerInputComponent->BindAction("Unpossess", EInputEvent::IE_Pressed, this, &ACCannon::UnpossessCannon);
	PlayerInputComponent->BindAxis("VerticalLook", this, &ACCannon::OnVerticalLook);
	PlayerInputComponent->BindAxis("HorizontalLook", this, &ACCannon::OnHorizontalLook);
}

void ACCannon::SpawnCannonProjectile()
{
	if (CannonProjectileClass)
	{
		CannonProjectile = ACProjectile::SpawnProjectile(this, CannonProjectileClass, "Muzzle_01");
		CannonProjectile->SetActorRotation(Camera->GetComponentRotation());

		if (CannonProjectile)
		{
			if (WidgetComponent->GetHitResult().GetActor())
				CannonProjectile->ShootProjectile(UKismetMathLibrary::GetDirectionUnitVector(GetMesh()->GetSocketLocation("Muzzle_01"), WidgetComponent->GetHitResult().ImpactPoint));
			else
				CannonProjectile->ShootProjectile(Camera->GetForwardVector());
		}
	} 
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
		
		CannonStateTypeChange(ECannonStateType::Possessed);

		ActivateCannonUtil();

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

		void DeactivateCannonUtil();

		GLog->Log("ACCannon::UnpossessCannon()");
	}
}

void ACCannon::OnFire()
{
	CannonStateTypeChange(ECannonStateType::Fire);
	
	Fire();
	SpawnCannonProjectile();

	GLog->Log("ACCannon::OnFire()");
}

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

void ACCannon::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{

}

void ACCannon::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ACCannon::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
}

void ACCannon::CannonStateTypeChange(ECannonStateType InType)
{
	PreviousCannonType = CurrentCannonType;
	CurrentCannonType = InType;

	if (OnCannonStateTypeChange.IsBound())
		OnCannonStateTypeChange.Broadcast(PreviousCannonType, CurrentCannonType);
}

