#pragma once

#include "CoreMinimal.h"
#include "Enemy/CEnemy.h"
#include "CEnemy_Helix.generated.h"

UCLASS()
class UE4POFOL_F_API ACEnemy_Helix : public ACEnemy
{
	GENERATED_BODY()

private:
	UPROPERTY()
	class ACProjectile* Projectile;

	UPROPERTY()
	TSubclassOf<class ACProjectile> ProjectileClass;

public:
	ACEnemy_Helix();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void OnAttack();

	void SpawnHelixProjectile();
};
