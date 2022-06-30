#pragma once

#include "CoreMinimal.h"
#include "Object/CTriggerVolume.h"
#include "CTriggerVolume_Spawner.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSpawnerEventTrigger, class AActor*, OverlappedActor, class AActor*, OtherActor);

UCLASS()
class UE4POFOL_F_API ACTriggerVolume_Spawner : public ACTriggerVolume
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FSpawnerEventTrigger OnSpawnerEventTrigger;
	
protected:
	ACTriggerVolume_Spawner();

private:
	virtual void OnBeginOverlap(class AActor* OverlappedActor, class AActor* OtherActor) override;
	virtual void OnEndOverlap(class AActor* OverlappedActor, class AActor* OtherActor) override;
};