/* MEMO: CAnimNotify_Onehand_FireSlash.h �ڵ� ��ġ ���� �Ϸ� */
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CAnimNotify_Onehandl_FireSlash.generated.h"


UCLASS()
class UE4POFOL_F_API UCAnimNotify_Onehandl_FireSlash : public UAnimNotify
{
	GENERATED_BODY()
	
private:
	FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)override;	
};
