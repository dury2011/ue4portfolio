#include "Notify/CAnimNotify_SpellMeteor.h"
#include "Global.h"
#include "Player/CPlayer.h"

FString UCAnimNotify_SpellMeteor::GetNotifyName_Implementation() const
{
	return "Spawn Spell Throw";
}

void UCAnimNotify_SpellMeteor::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->SpawnSpellMeteorWeapon();
}
