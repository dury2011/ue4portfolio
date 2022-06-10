#include "StaticObject/CElevator.h"
#include "Global.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemcomponent.h"
#include "StaticObject/CTriggerVolume_Elevator.h"
#include "GameFramework/Character.h"

ACElevator::ACElevator()
{
 	PrimaryActorTick.bCanEverTick = true;

	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>("Capsule Collision");
	RootComponent = CapsuleCollision;
	Elevator = CreateDefaultSubobject<UStaticMeshComponent>("Elevator");
	Elevator->SetupAttachment(CapsuleCollision);
	ElevatorRingGuard = CreateDefaultSubobject<UStaticMeshComponent>("Elevator Ring Guard");
	ElevatorRingGuard->SetupAttachment(Elevator);
	ParticleEmitter = CreateDefaultSubobject<UParticleSystemComponent>("Particle System");
	ParticleEmitter->SetupAttachment(CapsuleCollision);
}

void ACElevator::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> outActorArr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACTriggerVolume_Elevator::StaticClass(), outActorArr);

	for (int i = 0; i < outActorArr.Num(); i++)
		TriggerVolume = dynamic_cast<ACTriggerVolume_Elevator*>(outActorArr[i]);

	if (TriggerVolume)
		TriggerVolume->OnElevatorEventTrigger.AddDynamic(this, &ACElevator::ActivateElevator);
}

void ACElevator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACElevator::ActivateElevator(class AActor* InOverlappedActor, class AActor* InOtherActor)
{
	if (GetWorld()->GetFirstPlayerController() == Cast<ACharacter>(InOtherActor)->GetController())
	{
		FLatentActionInfo info;
		info.CallbackTarget = this;
	
		UKismetSystemLibrary::MoveComponentTo
		(
			CapsuleCollision,
			FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 33333.3f),
			GetActorRotation(),
			true,
			true,
			180.0f,
			false,
			EMoveComponentAction::Move,
			info
		);
	}
}

