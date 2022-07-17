#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Player/CPlayer.h"
#include "CAnimNotify_SetPlayerSpellFistT.generated.h"


UCLASS()
class UE4POFOL_F_API UCAnimNotify_SetPlayerSpellFistT : public UAnimNotify
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "Setting")
	EPlayerSpellFistType CurrentSpellFistType;

private:
	FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);
};
