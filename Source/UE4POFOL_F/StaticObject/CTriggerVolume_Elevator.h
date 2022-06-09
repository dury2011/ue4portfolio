#pragma once

#include "CoreMinimal.h"
#include "StaticObject/CTriggerVolume.h"
#include "CTriggerVolume_Elevator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FElevatorEventTrigger, class AActor*, OverlappedActor, class AActor*, OtherActor);

UCLASS()
class UE4POFOL_F_API ACTriggerVolume_Elevator : public ACTriggerVolume
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FElevatorEventTrigger OnElevatorEventTrigger;

protected:
	ACTriggerVolume_Elevator();

private:
	virtual void OnBeginOverlap(class AActor* OverlappedActor, class AActor* OtherActor) override;
	virtual void OnEndOverlap(class AActor* OverlappedActor, class AActor* OtherActor) override;
};
