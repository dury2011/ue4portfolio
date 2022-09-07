#include "Notify/CAnimNotifyState_YawFix.h"
#include "Global.h"
#include "Player/CPlayer.h"

FString UCAnimNotifyState_YawFix::GetNotifyName_Implementation() const
{
	return FString("Yaw Fix");
}

void UCAnimNotifyState_YawFix::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	CheckNull(MeshComp);

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->bUseControllerRotationYaw = false;
		
}

void UCAnimNotifyState_YawFix::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	CheckNull(MeshComp);

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->bUseControllerRotationYaw = true;
}