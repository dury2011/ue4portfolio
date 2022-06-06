//MEMO: UCAnimNotify_SpawnProjectileR.cpp 코드 배치 정리 완료
#include "Notify/CAnimNotify_SpawnProjectileR.h"
#include "Global.h"
#include "Player/CPlayer.h"

FString UCAnimNotify_SpawnProjectileR::GetNotifyName_Implementation() const
{
	return "SpawnSpellProjectile_R";
}

void UCAnimNotify_SpawnProjectileR::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->SpawnSpellProjectileR();
}