/* MEMO: CBullet.h 코드 배치 정리 완료 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon/CWeaponStructure.h"
#include "CProjectile.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FProjectileHit, UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

UCLASS()
class UE4POFOL_F_API ACProjectile : public AActor
{
	GENERATED_BODY()

public:
	//UPROPERTY(EditDefaultsOnly, Category = "Hit Actor Montage")
	//TArray<FDamageData> BulletHitDatas;

	//UPROPERTY(BlueprintAssignable)
	//FProjectileHit OnProjectileHit;

private:
	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleDefaultsOnly)
	class UProjectileMovementComponent* ProjectileComponent;

public:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	class USphereComponent* Sphere;

	//UPROPERTY(BlueprintReadWrite)
	//bool bBeginOverlapped = false;

	//UPROPERTY(BlueprintReadWrite)
	//bool bHitted = false;



public:
	ACProjectile();

protected:
	virtual void BeginPlay() override;

public:
	static ACProjectile* SpawnProjectile(ACharacter* InSpawner, TSubclassOf<class ACProjectile> InProjectileClass, FName InSpawnSocketName);

	void ShootProjectile(const FVector& InDirection);

	void DestroyProjectile();

private:
	UFUNCTION()
	void Hit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
