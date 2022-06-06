#include "StaticObject/CCrosshair_SpellThrow.h"
#include "Global.h"
#include "Player/CPlayer.h"
#include "Camera/CameraComponent.h"
#include "Component/CWidgetComponent.h"

ACCrosshair_SpellThrow::ACCrosshair_SpellThrow()
{

}

void ACCrosshair_SpellThrow::BeginPlay()
{
	Super::BeginPlay();
}

void ACCrosshair_SpellThrow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bAllocated)
	{
		FVector2D screenPosition;
		
		UGameplayStatics::ProjectWorldToScreen(UGameplayStatics::GetPlayerController(GetWorld(), 0), Player->WidgetComponent->GetHitResult().ImpactPoint, screenPosition);
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetMouseLocation(screenPosition.X, screenPosition.Y);
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;

		bAllocated = true;
	}

	if (GetOwner())
	{
		if (Player)
		{	
			CreateLineTrace(ECrosshairType::PortalOrThrow);

			FVector worldLocation;
			FVector worldDirection;

			UGameplayStatics::GetPlayerController(GetWorld(), 0)->DeprojectMousePositionToWorld(worldLocation, worldDirection);
			
			FVector lineTraceStart = worldLocation;
			FVector lineTraceEnd = lineTraceStart + worldDirection * 10000.0f;
			FHitResult hitResult;

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
				EDrawDebugTrace::ForOneFrame,
				hitResult,
				true,
				FColor::Green,
				FLinearColor::Green,
				0.0f
			);

			FVector playerLocation = FVector(Player->GetActorLocation().X, Player->GetActorLocation().Y, 0.0f);
			FVector tracerHitLocation = FVector(hitResult.ImpactPoint.X, hitResult.ImpactPoint.Y, 0.0f);
			
			float playerToImpactPointDistance = UKismetMathLibrary::Vector_Distance(playerLocation, tracerHitLocation);
			
			if (playerToImpactPointDistance >= 8000.0f)
			{
				//SetActorRotation(FRotator(0.0f, 0.0f, - PlayerCamera->GetComponentRotation().Roll));
				
				return;
			}

			ImpactPoint = hitResult.ImpactPoint;
			//SetActorRotation(FRotator(0.0f, 0.0f, - PlayerCamera->GetComponentRotation().Roll));
			SetActorLocation(hitResult.ImpactPoint);
		}
	}
}



