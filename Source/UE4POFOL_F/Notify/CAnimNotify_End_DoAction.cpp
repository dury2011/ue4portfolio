#include "Notify/CAnimNotify_End_DoAction.h"
#include "Global.h"
//#include "Component/CWeaponComponent.h"
#include "Weapon/CDoAction.h"
#include "Player/CPlayer.h"

FString UCAnimNotify_End_DoAction::GetNotifyName_Implementation() const
{
	return "End This Action";
}

void UCAnimNotify_End_DoAction::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	CheckNull(MeshComp);
	CheckNull(MeshComp->GetOwner());

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->EndThisAction();
}
