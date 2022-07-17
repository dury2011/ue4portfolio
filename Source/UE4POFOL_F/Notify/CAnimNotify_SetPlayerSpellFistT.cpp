#include "Notify/CAnimNotify_SetPlayerSpellFistT.h"
#include "Global.h"
#include "Player/CPlayer.h"

FString UCAnimNotify_SetPlayerSpellFistT::GetNotifyName_Implementation() const
{
	return FString("Set Player Spell Fist Type");
}

void UCAnimNotify_SetPlayerSpellFistT::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->Notify_SetCurrentPlayerSpellFistType(CurrentSpellFistType);
}
