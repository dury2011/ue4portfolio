#pragma once

#include "CoreMinimal.h"
#include "StaticObject/CTriggerVolume.h"
#include "CTriggerVolume_Cannon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCannonEventTrigger, class AActor*, OverlappedActor, class AActor*, OtherActor);

UCLASS()
class UE4POFOL_F_API ACTriggerVolume_Cannon : public ACTriggerVolume
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FCannonEventTrigger OnCannonEventTrigger;

protected:
	ACTriggerVolume_Cannon();

private:
	virtual void OnBeginOverlap(class AActor* OverlappedActor, class AActor* OtherActor) override;
	virtual void OnEndOverlap(class AActor* OverlappedActor, class AActor* OtherActor) override;
};
