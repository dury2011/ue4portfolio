/* MEMO: UCAnimNotify_DamageLaunchUp.cpp �ڵ� ��ġ ���� �Ϸ� */
#include "Notify/CAnimNotify_DamageLaucnhUp.h"
#include "Global.h"
#include "Player/CPlayer.h"


FString UCAnimNotify_DamageLaucnhUp::GetNotifyName_Implementation() const
{
	return "Damage Enemy Launch Up";
}

void UCAnimNotify_DamageLaucnhUp::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->Notify_OnSkillLaunch();
}