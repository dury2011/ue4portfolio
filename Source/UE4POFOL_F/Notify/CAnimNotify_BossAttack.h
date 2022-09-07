#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Enemy/CEnemy_Boss.h"
#include "CAnimNotify_BossAttack.generated.h"

UCLASS()
class UE4POFOL_F_API UCAnimNotify_BossAttack : public UAnimNotify
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	EBossAttackType BossAttackType;

private:
	FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);
};
