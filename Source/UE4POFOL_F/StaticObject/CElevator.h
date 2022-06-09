#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CElevator.generated.h"

UCLASS()
class UE4POFOL_F_API ACElevator : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* Elevator;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* ElevatorRingGuard;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UCapsuleComponent* CapsuleCollision;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UParticleSystemComponent* ParticleEmitter;

private:
	UPROPERTY()
	class ACTriggerVolume_Elevator* TriggerVolume;
	
public:	
	ACElevator();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void ActivateElevator(class AActor* InOverlappedActor, class AActor* InOtherActor);

	UFUNCTION(BlueprintImplementableEvent)
	void LiftStart();

	UFUNCTION(BlueprintImplementableEvent)
	void LiftEnd();
};
