#include "Notify/CAnimNotify_PortalDoorEntrance.h"
#include "Global.h"
#include "Player/CPlayer.h"

FString UCAnimNotify_PortalDoorEntrance::GetNotifyName_Implementation() const
{
	return "PortalEntrance";
}

void UCAnimNotify_PortalDoorEntrance::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->SpawnPortalDoorEntrance();
}

