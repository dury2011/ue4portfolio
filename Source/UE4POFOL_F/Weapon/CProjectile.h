/* MEMO: CBullet.h 코드 배치 정리 완료 22.08 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon/CWeaponStructure.h"
#include "CProjectile.generated.h"

UCLASS()
class UE4POFOL_F_API ACProjectile : public AActor
{
	GENERATED_BODY()

	//UPROPERTY(EditDefaultsOnly, Category = "Hit Actor Montage")
	//TArray<FDamageData> BulletHitDatas;

	//UPROPERTY(BlueprintAssignable)
	//FProjectileHit OnProjectileHit;

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class USphereComponent* Sphere;
	
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	class UStaticMeshComponent* Mesh;
	
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	class UProjectileMovementComponent* ProjectileComponent;

	//UPROPERTY(BlueprintReadWrite)
	//bool bBeginOverlapped = false;

	//UPROPERTY(BlueprintReadWrite)
	//bool bHitted = false;

private:

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Projectile Setting")
	int32 AttackDamage = 100;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile Setting")
	int32 AttackDamageDeviation = 50;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile Setting")
	bool IsCannonRangedProjectile = false;

	//UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Setting")
	//bool IsHoming = false;

	//int32 Homing_HitCount = 0;

public:
	ACProjectile();

	static ACProjectile* SpawnProjectile(ACharacter* InSpawner, TSubclassOf<class ACProjectile> InProjectileClass, FName InSpawnSocketName);
	static ACProjectile* SpawnProjectile(ACharacter* InSpawner, TSubclassOf<class ACProjectile> InProjectileClass, FVector InSpawnLocation);

	UFUNCTION(BlueprintCallable)
	void ShootProjectile(const FVector& InDirection);

	void DestroyProjectile();

	UFUNCTION(BlueprintCallable)
	void SetAttackDamage(int32 InAttackDamage);

private:
	UFUNCTION()
	void Hit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	virtual void BeginPlay() override;


};
