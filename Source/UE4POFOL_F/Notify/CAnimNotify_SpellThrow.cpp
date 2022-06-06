#include "Notify/CAnimNotify_SpellThrow.h"
#include "Global.h"
#include "Player/CPlayer.h"

FString UCAnimNotify_SpellThrow::GetNotifyName_Implementation() const
{
	return "Spawn Spell Throw";
}

void UCAnimNotify_SpellThrow::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->SpawnSpellThrowProjectile();
}