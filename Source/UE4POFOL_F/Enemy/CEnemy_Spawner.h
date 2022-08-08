#pragma once

#include "CoreMinimal.h"
#include "Enemy/CEnemy.h"
#include "CEnemy_Spawner.generated.h"

UCLASS()
class UE4POFOL_F_API ACEnemy_Spawner : public ACEnemy
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* StaticMeshSphere;

private:

	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* StaticMeshSpawner;

	UPROPERTY()
	class ACTriggerVolume_Spawner* TriggerVolume;

	int32 SpawnedEnemy = 0;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Spawner Setting")
	TSubclassOf<class ACEnemy> EnemyNormalClass;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Spawner Setting")
	TSubclassOf<class ACEnemy> EnemySpecialClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Enemy Spawner Setting")
	float EnemySpawnDelaySecond = 1.5f;

	FTimerHandle SpawnerTimer;
	FHitResult HitResult;

public:
	ACEnemy_Spawner();
	virtual void Tick(float DeltaTime) override;
	virtual void OnAttack() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

private:
	UFUNCTION()
	void ActivateEnemySpawner(class AActor* InOverlappedActor, class AActor* InOtherActor);

	void CreateLineTrace();
	void SpawnEnemy();
	
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)override;
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)override;
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)override;

protected:
	virtual void BeginPlay() override;

};
