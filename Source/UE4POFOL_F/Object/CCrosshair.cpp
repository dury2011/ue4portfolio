#include "Object/CCrosshair.h"
#include "Global.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"

ACCrosshair::ACCrosshair()
{
 	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>("SphereCollision");

	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>("ParticleSystem");
	ParticleSystem->SetupAttachment(SphereCollision);

	RootComponent = SphereCollision;
}

void ACCrosshair::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACCrosshair::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (GetOwner())
	{
		if (!IsPlayerRelativeAllocated)
		{
			Player = Cast<ACPlayer>(GetOwner());
			PlayerCamera = Cast<UCameraComponent>(GetOwner()->GetComponentByClass(UCameraComponent::StaticClass()));

			IsPlayerRelativeAllocated = true;
		}
	}
}

ACCrosshair* ACCrosshair::SpawnCrosshair(ACharacter* InSpawner, TSubclassOf<ACCrosshair> InCrosshairClass)
{
	FActorSpawnParameters params;
	params.Owner = InSpawner;
	
	return InSpawner->GetWorld()->SpawnActor<ACCrosshair>(InCrosshairClass, FVector::ZeroVector, FRotator::ZeroRotator, params);
}

void ACCrosshair::DestroyCrosshair()
{
	IsPlayerRelativeAllocated = false;
	
	if (UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor == true)
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = false;

	Destroy();
}

void ACCrosshair::CreateLineTrace(ECrosshairType InType)
{
	switch (InType)
	{
		case ECrosshairType::Normal :
		{
			/*UCameraComponent* camera*/ PlayerCamera = Cast<UCameraComponent>(GetOwner()->GetComponentByClass(UCameraComponent::StaticClass()));
				
			FTransform cameraTransform = /*camera*/ PlayerCamera->GetComponentToWorld();

			FVector lineTraceStart = cameraTransform.GetLocation();
			FVector lineTraceEnd = lineTraceStart + FQuat(cameraTransform.GetRotation()).GetForwardVector() * 10000.0f;

			TArray<AActor*> lineTraceIgnoreActor;
			lineTraceIgnoreActor.Add(GetOwner());

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

			break;
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
		case ECrosshairType::PortalOrThrow :
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

			break;
		}
		//case ECrosshairType::Throw :
		//{
		//	FTransform crosshairTransform = GetActorTransform();
		//
		//	FVector lineTraceStart = crosshairTransform.GetLocation();
		//	FVector lineTraceEnd = lineTraceStart + FQuat(crosshairTransform.GetRotation()).GetUpVector() * -10000.0f;
		//
		//	TArray<AActor*> lineTraceIgnoreActor;
		//	lineTraceIgnoreActor.Add(GetOwner());
		//
		//	UKismetSystemLibrary::LineTraceSingle
		//	(
		//		GetWorld(),
		//		lineTraceStart,
		//		lineTraceEnd,
		//		ETraceTypeQuery::TraceTypeQuery4,
		//		false,
		//		lineTraceIgnoreActor,
		//		EDrawDebugTrace::ForOneFrame,
		//		HitResult,
		//		true,
		//		FColor::Red,
		//		FLinearColor::Green,
		//		0.0f
		//	);
		//}

		default:
			break;
	}
}