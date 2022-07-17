#include "Notify/CAnimNotify_SetPlayerSkillType.h"
#include "Global.h"
#include "Player/CPlayer.h"

FString UCAnimNotify_SetPlayerSkillType::GetNotifyName_Implementation() const
{
	return FString("Set Player Skill Type");
}

void UCAnimNotify_SetPlayerSkillType::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->Notify_SetCurrentPlayerSkillType(CurrentSkillType);
}