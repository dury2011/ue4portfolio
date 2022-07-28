#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Interface/CInterface_PlayerState.h"
#include "CAnimNotify_NormalAttack.generated.h"

UCLASS()
class UE4POFOL_F_API UCAnimNotify_NormalAttack : public UAnimNotify
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "Setting")
	EPlayerNormalAttackType CurrentPlayerNormalAttackType;

private:
	FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation);
};
