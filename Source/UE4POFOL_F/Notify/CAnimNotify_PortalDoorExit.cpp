#include "Notify/CAnimNotify_PortalDoorExit.h"
#include "Global.h"
#include "Player/CPlayer.h"

FString UCAnimNotify_PortalDoorExit::GetNotifyName_Implementation() const
{
	return "PortalExit";
}

void UCAnimNotify_PortalDoorExit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->SpawnPortalDoorExit();
}
