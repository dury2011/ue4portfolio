/* MEMO: CRocket.h 코드 배치 정리 완료 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon/CWeaponStructure.h"
#include "CRocket.generated.h"

UCLASS()
class UE4POFOL_F_API ACRocket : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category = "Hit Actor Montage")
	TArray<FDamageData> RocketDamageDatas;

	UPROPERTY(VisibleDefaultsOnly)
	class USphereComponent* Sphere;

	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleDefaultsOnly)
	class UProjectileMovementComponent* Projectile;

	UPROPERTY(EditDefaultsOnly, Category = "Set Effect Asset")
	class UNiagaraSystem* RocketLocMark;

	UPROPERTY(EditDefaultsOnly, Category = "Set Effect Asset")
	class UParticleSystem* Explosion;

	UPROPERTY()
	FHitResult HitResult;
	
	UPROPERTY()
	TArray<AActor*>LockedOnActors;
	
	float DistanceToLand;

	FVector ImpactPointToLand;

	bool bLockedOn = false;

public:	
	ACRocket();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	
	void Shoot(const FVector& InDirection);

private:
	void CreateLineTraceToLand();

	void CheckLockedOn();

	void LockedOn();

	void SpawnMarkNiagara();

private:
	UFUNCTION()
	void Hit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	FORCEINLINE UProjectileMovementComponent* GetProjectile() { return Projectile; }
	FORCEINLINE bool IsLockedOn() { return bLockedOn == true; }
};
