#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CInterface_PlayerState.generated.h"

UINTERFACE(MinimalAPI)
class UCInterface_PlayerState : public UInterface
{
	GENERATED_BODY()
};

class UE4POFOL_F_API ICInterface_PlayerState
{
	GENERATED_BODY()

	
public:
	virtual float CurrentHp() { return 0.0f; };
	virtual float CurrentMp() { return 0.0f; };
	virtual float CurrentSp() { return 0.0f; };

	virtual float MaxHp() { return 0.0f; };
	virtual float MaxMp() { return 0.0f; };
	virtual float MaxSp() { return 0.0f; };
};
