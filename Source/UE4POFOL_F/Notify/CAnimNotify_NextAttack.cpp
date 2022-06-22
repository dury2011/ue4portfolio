#include "Notify/CAnimNotify_NextAttack.h"
#include "Global.h"
#include "Player/CPlayer.h"

FString UCAnimNotify_NextAttack::GetNotifyName_Implementation() const
{
	return FString("Next Attack");
}

void UCAnimNotify_NextAttack::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	CheckNull(MeshComp);
	
	 ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
	{
		if(player->GetbCanCombo())
			player->SetIncreaseIndex();
	}
}

