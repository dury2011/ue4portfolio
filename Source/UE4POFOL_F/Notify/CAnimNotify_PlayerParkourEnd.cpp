#include "Notify/CAnimNotify_PlayerParkourEnd.h"
#include "Global.h"
#include "Player/CPlayer.h"

FString UCAnimNotify_PlayerParkourEnd::GetNotifyName_Implementation() const
{
	return FString("Player End Parkour");
}

void UCAnimNotify_PlayerParkourEnd::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	CheckNull(MeshComp);

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->Notify_OnParkourFinish();
}