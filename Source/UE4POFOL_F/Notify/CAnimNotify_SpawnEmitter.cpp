/* MEMO: CAnimNotify_SpawnEmitter.cpp 코드 배치 정리 완료 */
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