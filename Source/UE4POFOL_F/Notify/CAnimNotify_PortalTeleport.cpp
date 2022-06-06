#include "Notify/CAnimNotify_PortalTeleport.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "Component/CWidgetComponent.h"
#include "Components/CapsuleComponent.h"
#include "Player/CPlayer.h"

FString UCAnimNotify_PortalTeleport::GetNotifyName_Implementation() const
{
	return "ProtalTeleport";
}

void UCAnimNotify_PortalTeleport::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
	{
		//FLatentActionInfo info;
		//info.CallbackTarget = this;
		//
		//FVector PortalTeleportLoc = player->WidgetComponent->GetHitResult().ImpactPoint;
		//
		//UKismetSystemLibrary::MoveComponentTo
		//(
		//	player->GetCapsuleComponent(),
		//	// TODO: 안좋다. 수정할 것
		//	FVector(PortalTeleportLoc.X, PortalTeleportLoc.Y, PortalTeleportLoc.Z + 90.0f),
		//	player->GetActorRotation(),
		//	true,
		//	false,
		//	0.1f,
		//	false,
		//	EMoveComponentAction::Move,
		//	info
		//);

		player->SetPlayerPortalLocation();
	}
}