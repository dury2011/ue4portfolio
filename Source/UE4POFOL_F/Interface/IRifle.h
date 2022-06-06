/* MEMO: IRifle.h 코드 배치 정리 완료 */
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IRifle.generated.h"

UINTERFACE(MinimalAPI)
class UIRifle : public UInterface
{
	GENERATED_BODY()
};

class UE4POFOL_F_API IIRifle
{
	GENERATED_BODY()

public:
	virtual class ACRifle* GetRifle() = 0;
};
