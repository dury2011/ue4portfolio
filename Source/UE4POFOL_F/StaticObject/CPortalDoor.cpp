#include "StaticObject/CPortalDoor.h"
#include "Global.h"
#include "Components/SphereComponent.h"
#include "Player/CPlayer.h"
#include "Camera/CameraComponent.h"

ACPortalDoor::ACPortalDoor()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereCollision = CreateDefaultSubobject<USphereComponent>("Sphere");

	RootComponent = SphereCollision;
}

void ACPortalDoor::BeginPlay()
{
	Super::BeginPlay();
	
	SetLifeSpan(5.0f);
}

void ACPortalDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

ACPortalDoor* ACPortalDoor::SpawnPortalDoor(ACharacter* InSpawner, const FVector& InSpawnLocation, TSubclassOf<class ACPortalDoor> InPortalDoorClass)
{
	FActorSpawnParameters params;
	params.Owner = InSpawner;

	ACPlayer* player = Cast<ACPlayer>(InSpawner);

	UCameraComponent* camera = Cast<UCameraComponent>(player->GetComponentByClass(UCameraComponent::StaticClass()));

	return InSpawner->GetWorld()->SpawnActor<ACPortalDoor>
		(
			InPortalDoorClass,
			InSpawnLocation,
			camera->GetComponentRotation(),
			params
		);
}

void ACPortalDoor::DestroyPortalDoor()
{
	Destroy();
}
