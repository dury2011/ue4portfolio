/* MEMO: CSpawner.h 코드 배치 정리 완료 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSpawner.generated.h"

UCLASS()
class UE4POFOL_F_API ACSpawner : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Spawner Setting")
	float Hp = 15000.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Spawner Setting")
	float SpawnDelaySecond = 5.0f;

private:
	struct FDamaged
	{
		float DamageAmount;
		//FActionDamageEvent* DamageEvent;
		class AController* EventInstigator;
		class AActor* DamageCauser;
	} Damaged;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner Setting")
	TSubclassOf<class ACEnemy> EnemyNormalClass;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner Setting")
	TSubclassOf<class ACEnemy> EnemySpecialClass;
	
	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* StaticMeshSpawner;

	UPROPERTY(VisibleDefaultsOnly)
	class UCapsuleComponent* CapsuleCollision;

	UPROPERTY()
	class ACTriggerVolume_Spawner* TriggerVolume;
	
	int32 SpawnedEnemy = 0;

	FTimerHandle SpawnerTimer;

public:
	ACSpawner();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void ActivateSpawner(class AActor* InOverlappedActor, class AActor* InOtherActor);
public:
	void SpawnEnemy();
	void DestroySpawner();

	UFUNCTION(BlueprintImplementableEvent, Category = "CSpawner")
	void EnemySpawnParticleEffect();
};
