/* MEMO: CAnimNotify_SpawnEmitter.cpp �ڵ� ��ġ ���� �Ϸ� */
#include "Notify/CAnimNotify_SpawnEmitter.h"
#include "Global.h"
#include "Player/CPlayer.h"

FString UCAnimNotify_SpawnEmitter::GetNotifyName_Implementation() const
{
	return "Spawn Emitter";
}

void UCAnimNotify_SpawnEmitter::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
	{


	}
}