/* MEMO: CAnimNotify_BossCallFriend.cpp 코드 배치 정리 완료 */
#include "Notify/CAnimNotify_BossCallFriend.h"
#include "Global.h"
#include "Boss/CBoss.h"

FString UCAnimNotify_BossCallFriend::GetNotifyName_Implementation() const
{
	return FString("Boss Call Friend");
}

void UCAnimNotify_BossCallFriend::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACBoss* boss = Cast<ACBoss>(MeshComp->GetOwner());

	if (boss)
		boss->SpawnFriend();
}