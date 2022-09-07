
#pragma once

#include "CoreMinimal.h"
#include "Enemy/CEnemy.h"
#include "CEnemy_Boss.generated.h"


UENUM(BlueprintType)
enum class EBossAttackType : uint8
{
	NormalAttack, BoundUpAttack, AirborneAttack, GroggyAttack, FinalAttack, Max
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossAttack, EBossAttackType, InType);

UCLASS()
class UE4POFOL_F_API ACEnemy_Boss : public ACEnemy
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	bool CanRotateToOpponent = true;
	
	FOnBossAttack OnBossAttack;

private:
	FTimerHandle OnAttackTimer;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Enemy_Boss Setting")
	float NormalAttackRange = 1000.0;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* BoxCollisionAttack;

	FTimerHandle SearchTimer;
	FTimerHandle FinishSearchTimer;
	TArray<FVector>SearchedLocations;

	EBossAttackType CurrentBossAttackType;

	bool CanBind = true;
	bool Once_IsBossFriendCalled = false;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TArray<FActionData> NormalAttackDatas;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TArray<FActionData> RangeAttackDatas;

	//UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	//TArray<FActionData> SpecialAttackDatas;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TArray<FActionData> NormalSpecialAttackDatas;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TArray<FActionData> RangeSpecialAttackDatas;

private:
	ACEnemy_Boss();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void OnAttack() override;
	
	virtual void OnMontageEnded(UAnimMontage* InMontage, bool InInterrupted) override;	

private:
	UFUNCTION()
	void OnBoxCollisionAttackBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxCollisionAttackEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	// Notify
	void Notify_BossAttack(EBossAttackType InType);

	void Notify_SpawnBossRangeAttack1Projectile(TSubclassOf<ACProjectile> InProjectileClass);

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnRangeSpecialAttackEnemySpawnBossEffect();
};
