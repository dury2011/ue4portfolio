/* MEMO: CBall.h 코드 배치 정리 완료 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon/CWeaponStructure.h"
#include "CBall.generated.h"

UCLASS()
class UE4POFOL_F_API ACBall : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = "Hit Actor Montage")
	TArray<FDamageData> BallHitDatas;

private:
	UPROPERTY(VisibleDefaultsOnly)
	class UCapsuleComponent* Capsule;

	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* Mesh;
	
	UPROPERTY(VisibleDefaultsOnly)
	class UProjectileMovementComponent* Projectile;

	UPROPERTY(EditDefaultsOnly, Category = "Set Effect Asset")
	class UNiagaraSystem* BallLocMark;

	UPROPERTY(EditDefaultsOnly, Category = "Set Effect Asset")
	class UParticleSystem* Explosion;

	UPROPERTY()
	FHitResult HitResult;

	float DistanceToLand;

	FVector ImpactPointToLand;

	FTimerHandle Timer;

public:	
	ACBall();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void Shoot(const FVector& InDirection);

private:
	void CreateLineTraceToLand();

	void SpawnMarkNiagara();

private:
	UFUNCTION()
	void Hit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
