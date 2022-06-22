#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CAnimNotify_NextAttack.generated.h"

UCLASS()
class UE4POFOL_F_API UCAnimNotify_NextAttack : public UAnimNotify
{
	GENERATED_BODY()
	
private:
	FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation);
};
