#include "Notify/CAnimNotify_Begin_DoAction.h"
#include "CAnimNotify_Begin_DoAction.h"
#include "Global.h"
//#include "Component/CWeaponComponent.h"
#include "Player/CPlayer.h"

FString UCAnimNotify_Begin_DoAction::GetNotifyName_Implementation() const
{
	return "Begin Next Action";
}

void UCAnimNotify_Begin_DoAction::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	Super::Notify(MeshComp, Animation);
	CheckNull(MeshComp);
	CheckNull(MeshComp->GetOwner());

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->Notify_BeginNextAction();
}
