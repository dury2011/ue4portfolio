/* MEMO: UCAnimNotify_Onehand_FireStorm.cpp �ڵ� ��ġ ���� �Ϸ� */
#include "Notify/CAnimNotify_Onehand_FireStorm.h"
#include "Global.h"
#include "Player/CPlayer.h"

FString UCAnimNotify_Onehand_FireStorm::GetNotifyName_Implementation() const
{
	return "Fire Storm";
}

void UCAnimNotify_Onehand_FireStorm::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->SpawnEmitter2();
}