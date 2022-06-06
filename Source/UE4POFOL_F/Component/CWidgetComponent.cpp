#include "Component/CWidgetComponent.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "Player/CPlayer.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/PoseableMeshComponent.h"

UCWidgetComponent::UCWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
}

void UCWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(bCrosshairVisible)
		CreateLineTrace();
}

void UCWidgetComponent::CreateLineTrace()
{
	UCameraComponent* camera = CHelpers::GetComponent<UCameraComponent>(OwnerCharacter, "Camera");

	FTransform cameraTransform = camera->GetComponentToWorld();

	FVector lineTraceStart = cameraTransform.GetLocation();
	FVector lineTraceEnd = lineTraceStart + FQuat(cameraTransform.GetRotation()).GetForwardVector() * 10000.0f;
	
	TArray<AActor*> lineTraceIgnoreActor;
	lineTraceIgnoreActor.Add(OwnerCharacter);

	//TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes; // ��Ʈ ������ ������Ʈ ������.
	//TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	////TEnumAsByte<EObjectTypeQuery> WorldDynamic = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic);
	//objectTypes.Add(Pawn);
	////objectTypes.Add(WorldDynamic);

	UKismetSystemLibrary::LineTraceSingle
	(
		GetWorld(),
		lineTraceStart,
		lineTraceEnd,
		ETraceTypeQuery::TraceTypeQuery4,
		false,
		lineTraceIgnoreActor,
		EDrawDebugTrace::ForOneFrame,
		HitResult,
		true,
		FColor::Red,
		FLinearColor::Green,
		0.0f
	);

	//UKismetSystemLibrary::LineTraceSingleForObjects
	//(
	//	GetWorld(),
	//	lineTraceStart,
	//	lineTraceEnd,
	//	objectTypes,
	//	false,
	//	lineTraceIgnoreActor,
	//	EDrawDebugTrace::ForDuration,
	//	HitResult,
	//	true,
	//	FColor::Red,
	//	FLinearColor::Green,
	//	0.0f
	//);
}

bool UCWidgetComponent::CanCrosshairWidgetColorChange()
{
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Enemy", actors);
	
	for (AActor* actor : actors)
	{
		if (actor)
		{
			if (HitResult.Actor == actor)
				if (HitResult.bBlockingHit)
					return true;
				else
					return false;
		}
		else
			return false;
	}
	
	return false;
}
