#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CInterface_Interaction.generated.h"

UINTERFACE(MinimalAPI)
class UCInterface_Interaction : public UInterface
{
	GENERATED_BODY()
};


class UE4POFOL_F_API ICInterface_Interaction
{
	GENERATED_BODY()
	
private:
	int32 TargettedCount;

public:
	void IncrementTargettedCount() { ++TargettedCount; };
	void decrementTargettedCount() { if (TargettedCount <= 0) return; else --TargettedCount; }

	int32 GetTargettedCount() { return TargettedCount; }

};
