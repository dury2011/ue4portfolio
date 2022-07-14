#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CInterface_Item.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Hp, Mp, Sp, Max
};

UINTERFACE(MinimalAPI)
class UCInterface_Item : public UInterface
{
	GENERATED_BODY()
};

class UE4POFOL_F_API ICInterface_Item
{
	GENERATED_BODY()

public:
	virtual void SetCurrentItemType(EItemType InType) { };
	virtual EItemType GetCurrentItemType() { return EItemType::Max; };
	virtual float ActivateItemAbility() { return 0.0f; };
};
