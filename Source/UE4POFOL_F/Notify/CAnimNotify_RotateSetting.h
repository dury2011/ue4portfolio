#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CAnimNotify_RotateSetting.generated.h"

UCLASS()
class UE4POFOL_F_API UCAnimNotify_RotateSetting : public UAnimNotify
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere)
	bool CanRotate = false;
	
private:
	FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation);	
};
