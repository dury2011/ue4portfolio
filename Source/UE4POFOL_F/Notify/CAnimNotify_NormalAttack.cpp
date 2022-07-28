#include "Notify/CAnimNotify_NormalAttack.h"
#include "Global.h"
#include "Player/CPlayer.h"

FString UCAnimNotify_NormalAttack::GetNotifyName_Implementation() const
{
	return FString("Player Normal Attack");
}

void UCAnimNotify_NormalAttack::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	CheckNull(MeshComp);

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
	{	
		player->Notify_SetCurrentPlayerNormalAttackType(CurrentPlayerNormalAttackType);
		player->Notify_OnNormalAttack();
	}
}