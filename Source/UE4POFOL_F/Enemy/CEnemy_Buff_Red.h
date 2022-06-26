#pragma once

#include "CoreMinimal.h"
#include "Enemy/CEnemy.h"
#include "CEnemy_Buff_Red.generated.h"

UCLASS()
class UE4POFOL_F_API ACEnemy_Buff_Red : public ACEnemy
{
	GENERATED_BODY()
	
private:

public:


public:
	ACEnemy_Buff_Red();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

};
