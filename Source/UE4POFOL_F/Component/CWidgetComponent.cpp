#include "Component/CWidgetComponent.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "Player/CPlayer.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"

UCWidgetComponent::UCWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());

	PlayerInterface = Cast<ICInterface_PlayerState>(OwnerCharacter);

	if (PlayerInterface)
	{
		MaxHp = PlayerInterface->MaxHp();
		MaxMp = PlayerInterface->MaxMp();
		MaxSp = PlayerInterface->MaxSp();
	}
}

void UCWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PlayerInterface)
	{
		CurrentHp = PlayerInterface->CurrentHp();
		CurrentMp = PlayerInterface->CurrentMp();
		CurrentSp = PlayerInterface->CurrentSp();
	}

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

	//TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes; // 히트 가능한 오브젝트 유형들.
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
		EDrawDebugTrace::None,
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
				//if (HitResult.bBlockingHit)
					return true;
				else
					return false;
		}
		else
			return false;
	}
	
	return false;
}

//void UCWidgetComponent::CheckWarnningText()
//{
//	if (PlayerInterface)
//	{
//		if ((CurrentHp / MaxHp <= 0.3f) && !IsEventCalled)
//		{
//			WarnningText_LowHp();
//
//			IsEventCalled = true;
//		}
//		else if ((CurrentHp / MaxHp > 0.3f) && IsEventCalled)
//		{
//			IsEventCalled = false;
//		}
//		if ((CurrentMp / MaxMp <= 0.3f) && !IsEventCalled)
//		{
//			WarnningText_LowMp();
//
//			IsEventCalled = true;
//		}
//		else if ((CurrentMp / MaxMp > 0.3f) && IsEventCalled)
//		{
//			IsEventCalled = false;
//		}
//	}
//}

//float UCWidgetComponent::GetCurrentHpPercentage()
//{
//	return CurrentHp / MaxHp;
//}
//
//float UCWidgetComponent::GetCurrentMpPercentage()
//{
//	return CurrentMp / MaxMp;
//}
