#pragma once

#include "CoreMinimal.h"
#include "Object/CCrosshair.h"
#include "CCrosshair_SpellThrow.generated.h"

UCLASS()
class UE4POFOL_F_API ACCrosshair_SpellThrow : public ACCrosshair
{
	GENERATED_BODY()
private:
	bool bAllocated = false;
	FVector ImpactPoint;

	UPROPERTY(EditDefaultsOnly, Category = "ACCrosshair_SpellThrow Setting")
	float SpawnProjectileZLocation = 2000.0f;

public:
	ACCrosshair_SpellThrow();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE FVector GetImpactPoint() { return FVector(ImpactPoint.X, ImpactPoint.Y, ImpactPoint.Z + SpawnProjectileZLocation); }

};
