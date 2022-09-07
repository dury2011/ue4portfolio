#include "Notify/CAnimNotify_RotateSetting.h"
#include "Global.h"
#include "Enemy/CEnemy_Boss.h"

FString UCAnimNotify_RotateSetting::GetNotifyName_Implementation() const
{
	return FString("Rotation Setting");
}

void UCAnimNotify_RotateSetting::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	CheckNull(MeshComp);

	ACEnemy_Boss* boss = Cast<ACEnemy_Boss>(MeshComp->GetOwner());

	if (boss)
		boss->CanRotateToOpponent = CanRotate;

}