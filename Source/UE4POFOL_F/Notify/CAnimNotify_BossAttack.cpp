#include "Notify/CAnimNotify_BossAttack.h"
#include "Global.h"
#include "Enemy/CEnemy_Boss.h"

FString UCAnimNotify_BossAttack::GetNotifyName_Implementation() const
{
	return FString("Boss Attack");
}

void UCAnimNotify_BossAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACEnemy_Boss* boss = Cast<ACEnemy_Boss>(MeshComp->GetOwner());

	if (boss)
		boss->Notify_BossAttack(BossAttackType);
}