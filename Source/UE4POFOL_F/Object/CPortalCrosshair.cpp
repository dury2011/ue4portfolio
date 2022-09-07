#include "Object/CPortalCrosshair.h"
#include "Global.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystem.h"
#include "Player/CPlayer.h"
#include "Component/CWidgetComponent.h"
#include "Camera/CameraComponent.h"

ACPortalCrosshair::ACPortalCrosshair()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>("Sphere");

	RootComponent = SphereCollision;

	//ConstructorHelpers::FObjectFinder<UParticleSystem> asset(TEXT("ParticleSystem'/Game/ParagonGideon/FX/Particles/Gideon/Abilities/ProjectileMeteor/FX/P_Gideon_RMB_Targeting.P_Gideon_RMB_Targeting'"));

	//if (asset.Succeeded())
	//	ParticleSystem = asset.Object;

	//ParticleSystem->AddToRoot();
	
}

void ACPortalCrosshair::BeginPlay()
{
	Super::BeginPlay();
}

void ACPortalCrosshair::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (GetOwner())
	{
		Player = Cast<ACPlayer>(GetOwner());

		CreateLineTrace();
	}

	if (Player)
	{
		UCameraComponent* camera = Cast<UCameraComponent>(Player->GetComponentByClass(UCameraComponent::StaticClass()));

		FTransform cameraTransform = camera->GetComponentToWorld();

		FVector start = cameraTransform.GetLocation();
		FVector end = start + FQuat(cameraTransform.GetRotation()).GetForwardVector() * 2000.0f;

		//SetActorRotation(UKismetMathLibrary::RInterpTo(GetActorRotation(), camera->GetComponentRotation(), DeltaTime, 10.0f));
		//SetActorLocation(UKismetMathLibrary::VInterpTo(GetActorLocation(), end, DeltaTime, 10.0f));

		SetActorRotation(camera->GetComponentRotation());
		////SetActorLocation(FVector(HitResult.ImpactPoint.X, HitResult.ImpactPoint.Y, Player->GetActorLocation().Z));
		SetActorLocation(end);
	}
}

void ACPortalCrosshair::CreateLineTrace()
{
	FTransform crosshairTransform = GetActorTransform();

	FVector lineTraceStart = crosshairTransform.GetLocation();
	FVector lineTraceEnd = lineTraceStart + FQuat(crosshairTransform.GetRotation()).GetUpVector() * -10000.0f;

	TArray<AActor*> lineTraceIgnoreActor;
	lineTraceIgnoreActor.Add(GetOwner());

	UKismetSystemLibrary::LineTraceSingle
	(
		GetWorld(),
		lineTraceStart,
		lineTraceEnd,
		ETraceTypeQuery::TraceTypeQuery4,
		false,
		lineTraceIgnoreActor,
		EDrawDebugTrace::None,
		HitResult,
		true,
		FColor::Red,
		FLinearColor::Green,
		0.0f
	);
}

ACPortalCrosshair* ACPortalCrosshair::SpawnPortalCrosshair(ACharacter* InSpawner, TSubclassOf<class ACPortalCrosshair> InPortalCrosshairClass)
{
	FActorSpawnParameters params;
	params.Owner = InSpawner;

	ACPlayer* player = Cast<ACPlayer>(InSpawner);

	return InSpawner->GetWorld()->SpawnActor<ACPortalCrosshair>
	(
		InPortalCrosshairClass,
		FVector::ZeroVector,//(player->WidgetComponent->GetHitResult().ImpactPoint.X, player->WidgetComponent->GetHitResult().ImpactPoint.Y, InSpawner->GetActorLocation().Z),
		FRotator::ZeroRotator,
		params
	);
}

void ACPortalCrosshair::DestroyPortalCrosshair()
{
	Destroy();
}



