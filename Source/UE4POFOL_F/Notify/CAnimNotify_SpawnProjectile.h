#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CAnimNotify_SpawnProjectile.generated.h"

UCLASS()
class UE4POFOL_F_API UCAnimNotify_SpawnProjectile : public UAnimNotify
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "Setting")
	TSubclassOf<class ACProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Setting")
	FName SpawnSocketName;

	/* Activate Projectile Shoot */
	UPROPERTY(EditAnywhere, Category = "Setting")
	bool CanActivateShoot = true;

	UPROPERTY(EditAnywhere, Category = "Setting")
	bool ShootSpawnedLocationFoward = false;

	UPROPERTY(EditAnywhere, Category = "Setting")
	float ShootSpeed = 5000.0f;

	UPROPERTY()
	class ACProjectile* Projectile;

private:
	FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);
};
