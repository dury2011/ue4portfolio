#include "Notify/CAnimNotify_SpellFistAttack.h"
#include "Global.h"
#include "Player/CPlayer.h"

FString UCAnimNotify_SpellFistAttack::GetNotifyName_Implementation() const
{
	return FString("Player Spell Fist Attack");
}

void UCAnimNotify_SpellFistAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->Notify_OnSpellFistAttack();
}