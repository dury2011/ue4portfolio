#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Player/CPlayer.h"
#include "CAnimNotify_SetPlayerSkillType.generated.h"

UCLASS()
class UE4POFOL_F_API UCAnimNotify_SetPlayerSkillType : public UAnimNotify
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "Setting")
	EPlayerSkillType CurrentSkillType;
	
private:
	FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);	
};
