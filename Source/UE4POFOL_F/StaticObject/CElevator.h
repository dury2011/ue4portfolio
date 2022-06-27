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
	UPROPERTY(EditDefaultsOnly, Category = "Elevator Setting")
	TSubclassOf<class ACEnemy> EnemyHelixClass;

	UPROPERTY(EditDefaultsOnly, Category = "Elevator Setting")
	TSubclassOf<class ACEnemy> EnemyMeleeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Elevator Setting")
	TSubclassOf<class ACEnemy> EnemyRifleClass;

	UPROPERTY(EditDefaultsOnly, Category = "Elevator Setting")
	TSubclassOf<class ACEnemy> EnemyBuffRedClass;

	UPROPERTY()
	class ACTriggerVolume_Elevator* TriggerVolume;

	FTimerHandle Timer;
	int32 EnemyCount = 0;
	
public:	
	ACElevator();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void ActivateElevator(class AActor* InOverlappedActor, class AActor* InOtherActor);

	void SpawnEnemy();

	UFUNCTION(BlueprintImplementableEvent)
	void LiftStart();

	UFUNCTION(BlueprintImplementableEvent)
	void LiftEnd();
};
