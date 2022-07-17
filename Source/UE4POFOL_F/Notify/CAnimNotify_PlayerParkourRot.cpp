#include "Notify/CAnimNotify_PlayerParkourRot.h"
#include "Global.h"
#include "Player/CPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

FString UCAnimNotify_PlayerParkourRot::GetNotifyName_Implementation() const
{
	return FString("Player Parkour Rotation");
}

void UCAnimNotify_PlayerParkourRot::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	CheckNull(MeshComp);

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->Notify_ParkourRotation();
}