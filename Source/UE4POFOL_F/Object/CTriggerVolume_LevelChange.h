#pragma once

#include "CoreMinimal.h"
#include "Object/CTriggerVolume.h"
#include "CTriggerVolume_LevelChange.generated.h"

UCLASS()
class UE4POFOL_F_API ACTriggerVolume_LevelChange : public ACTriggerVolume
{
	GENERATED_BODY()
	
protected:
	ACTriggerVolume_LevelChange();

private:
	virtual void OnBeginOverlap(class AActor* OverlappedActor, class AActor* OtherActor) override;
	virtual void OnEndOverlap(class AActor* OverlappedActor, class AActor* OtherActor) override;

};
