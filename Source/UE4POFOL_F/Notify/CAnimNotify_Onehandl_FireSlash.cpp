/* MEMO: CAnimNotify_Onehand_FireSlash.cpp �ڵ� ��ġ ���� �Ϸ� */
#include "Notify/CAnimNotify_Onehandl_FireSlash.h"
#include "Global.h"
#include "Player/CPlayer.h"

FString UCAnimNotify_Onehandl_FireSlash::GetNotifyName_Implementation() const
{
	return "FireSlash";
}

void UCAnimNotify_Onehandl_FireSlash::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->SpawnEmitter();
}