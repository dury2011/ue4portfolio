#include "Notify/CAnimNotify_EnemyAttack.h"
#include "Global.h"
#include "Enemy/CEnemy.h"

FString UCAnimNotify_EnemyAttack::GetNotifyName_Implementation() const
{
	return FString("Enemy Attack");
}

void UCAnimNotify_EnemyAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACEnemy* enemy = Cast<ACEnemy>(MeshComp->GetOwner());

	if (enemy)
		if (enemy->OnEnemyAttack.IsBound())
			enemy->OnEnemyAttack.Broadcast();
}
