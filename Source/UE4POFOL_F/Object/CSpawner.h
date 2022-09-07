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
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UCapsuleComponent* CapsuleCollision;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class UStaticMeshComponent* StaticMeshSphere;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Spawner Setting")
	class UWidgetComponent* HealthBarWidgetComponent;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Spawner Setting")
	float Hp = 3000;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Spawner Setting")
	float SpawnDelaySecond = 5.0f;

	UPROPERTY(BlueprintReadWrite)
	bool CanSpawn = true;

	UPROPERTY(BlueprintReadOnly)
	FHitResult HitResult;

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
	
	//UPROPERTY(VisibleDefaultsOnly)
	//class UStaticMeshComponent* StaticMeshSpawner;

	UPROPERTY()
	class ACTriggerVolume_Spawner* TriggerVolume;
	
	int32 SpawnedEnemy = 0;

	FTimerHandle HealthBarTimer;
	FTimerHandle SpawnerTimer;


	TMap<UUserWidget*, FVector> HitNumbers;

public:
	ACSpawner();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void StoreHitNumber(UUserWidget* InHitNumber, FVector InLocation);

	UFUNCTION()
	void UpdateHitNumbers();

	UFUNCTION()
	void DestroyHitNumber(UUserWidget* InHitNumber);

	void ShowHealthBar();
	void HideHealthBar();

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

	void CreateLineTrace();
public:
	void SpawnEnemy();
	void DestroySpawner();

	UFUNCTION(BlueprintImplementableEvent, Category = "CSpawner")
	void EnemySpawnParticleEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitNumber(int32 InDamage, FVector InHitLocation);

	UFUNCTION(BlueprintImplementableEvent, Category = "CSpawner")
	void ActivateDestroySpawnerEffect();
};
