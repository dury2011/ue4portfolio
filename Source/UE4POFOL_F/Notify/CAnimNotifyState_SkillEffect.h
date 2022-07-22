#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CAnimNotifyState_SkillEffect.generated.h"

UCLASS()
class UE4POFOL_F_API UCAnimNotifyState_SkillEffect : public UAnimNotifyState
{
	GENERATED_BODY()
	
private:
	FString GetNotifyName_Implementation() const override;
	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;	
};
