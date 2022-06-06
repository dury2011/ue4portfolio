#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CAnimNotify_End_DoAction.generated.h"

UCLASS()
class UE4POFOL_F_API UCAnimNotify_End_DoAction : public UAnimNotify
{
	GENERATED_BODY()

private:
	FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);
	
};
