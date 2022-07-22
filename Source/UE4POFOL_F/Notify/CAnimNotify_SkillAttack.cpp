#include "Notify/CAnimNotify_SkillAttack.h"
#include "Global.h"
#include "Player/CPlayer.h"

FString UCAnimNotify_SkillAttack::GetNotifyName_Implementation() const
{
	return FString("Skill Attack");
}

void UCAnimNotify_SkillAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->Notify_OnSkillAttack();
}
