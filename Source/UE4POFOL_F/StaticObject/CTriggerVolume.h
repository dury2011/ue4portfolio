#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "CTriggerVolume.generated.h"

UCLASS()
class UE4POFOL_F_API ACTriggerVolume : public ATriggerVolume
{
	GENERATED_BODY()

private:
	bool bCanTrigger = false;
	
protected:
	ACTriggerVolume();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	virtual void OnBeginOverlap(class AActor* OverlappedActor, class AActor* OtherActor);

	UFUNCTION()
	virtual void OnEndOverlap(class AActor* OverlappedActor, class AActor* OtherActor);

	FORCEINLINE void SetbCanTrigger(bool Inbool) { bCanTrigger = Inbool; }
	FORCEINLINE bool GetbCanTrigger() { return bCanTrigger; }

};
