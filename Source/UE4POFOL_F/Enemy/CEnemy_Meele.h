#pragma once

#include "CoreMinimal.h"
#include "Enemy/CEnemy.h"
#include "CEnemy_Meele.generated.h"

UCLASS()
class UE4POFOL_F_API ACEnemy_Meele : public ACEnemy
{
	GENERATED_BODY()

public:
	ACEnemy_Meele();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void OnAttack() override;
};
