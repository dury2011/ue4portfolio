/* MEMO: CAnimNotify_ShootRocket.cpp �ڵ� ��ġ ���� �Ϸ� */
#include "Notify/CAnimNotify_ShootRocket.h"
#include "Global.h"
#include "Boss/CBoss.h"

FString UCAnimNotify_ShootRocket::GetNotifyName_Implementation() const
{
	return "ShootRocket";
}

void UCAnimNotify_ShootRocket::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	ACBoss* boss = Cast<ACBoss>(MeshComp->GetOwner());

	if (boss)
		boss->ShootRocket();
}

