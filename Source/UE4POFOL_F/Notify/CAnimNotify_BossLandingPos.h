/* MEMO: CAnimNotify_BossLandingPos.h 코드 배치 정리 완료 */
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CAnimNotify_BossLandingPos.generated.h"

UCLASS()
class UE4POFOL_F_API UCAnimNotify_BossLandingPos : public UAnimNotify
{
	GENERATED_BODY()
private:
	UPROPERTY()
	class ACBoss* Boss;

private:
	FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
