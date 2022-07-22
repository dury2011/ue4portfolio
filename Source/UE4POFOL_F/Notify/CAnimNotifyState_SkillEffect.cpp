#include "Notify/CAnimNotifyState_SkillEffect.h"
#include "Global.h"
#include "Player/CPlayer.h"

FString UCAnimNotifyState_SkillEffect::GetNotifyName_Implementation() const
{
	return FString("Player Skill Effect");
}

void UCAnimNotifyState_SkillEffect::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	CheckNull(MeshComp);
	
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->SpawnSkillEffect();
}

void UCAnimNotifyState_SkillEffect::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->DestroySkillEffect();
}