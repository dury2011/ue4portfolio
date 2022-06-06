//MEMO: UCAnimNotify_SpawnProjectileL.h �ڵ� ��ġ ���� �Ϸ�
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CAnimNotify_SpawnProjectileL.generated.h"


UCLASS()
class UE4POFOL_F_API UCAnimNotify_SpawnProjectileL : public UAnimNotify
{
	GENERATED_BODY()
	
private:
	FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);
};
