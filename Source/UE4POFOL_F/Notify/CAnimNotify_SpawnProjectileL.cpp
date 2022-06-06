//MEMO: UCAnimNotify_SpawnProjectileL.cpp �ڵ� ��ġ ���� �Ϸ�
#include "Notify/CAnimNotify_SpawnProjectileL.h"
#include "Global.h"
#include "Player/CPlayer.h"

FString UCAnimNotify_SpawnProjectileL::GetNotifyName_Implementation() const
{
	return "SpawnSpellProjectile_L";
}

void UCAnimNotify_SpawnProjectileL::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->SpawnSpellProjectileL();
}

