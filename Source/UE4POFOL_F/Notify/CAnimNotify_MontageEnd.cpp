#include "Notify/CAnimNotify_MontageEnd.h"
#include "Global.h"
#include "Enemy/CEnemy.h"

FString UCAnimNotify_MontageEnd::GetNotifyName_Implementation() const
{
	return FString("End Montage");
}

void UCAnimNotify_MontageEnd::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACEnemy* enemy = Cast<ACEnemy>(MeshComp->GetOwner());

	//if (enemy)
		//enemy->MontageEnded();
}
