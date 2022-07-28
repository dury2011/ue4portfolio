#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UE4POFOL_FGameModeBase.generated.h"

UCLASS()
class UE4POFOL_F_API AUE4POFOL_FGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	class ACTriggerVolume_LevelChange* TriggerVolume;

public:
	AUE4POFOL_FGameModeBase();

private:
	virtual void StartPlay() override;
};
