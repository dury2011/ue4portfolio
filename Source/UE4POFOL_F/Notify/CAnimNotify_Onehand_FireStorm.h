/* MEMO: UCAnimNotify_Onehand_FireStorm.h �ڵ� ��ġ ���� �Ϸ� */
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CAnimNotify_Onehand_FireStorm.generated.h"

UCLASS()
class UE4POFOL_F_API UCAnimNotify_Onehand_FireStorm : public UAnimNotify
{
	GENERATED_BODY()
	
private:
	FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
