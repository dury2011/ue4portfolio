#include "Notify/CAnimNotify_EnemyEffectWeapon.h"
#include "Global.h"
#include "Enemy/CEnemy.h"

FString UCAnimNotify_EnemyEffectWeapon::GetNotifyName_Implementation() const
{
	return "Spawn Enemy Effect Weapon";
}

void UCAnimNotify_EnemyEffectWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);
	CheckNull(MeshComp->GetOwner());

	ACEnemy* enemy = Cast<ACEnemy>(MeshComp->GetOwner());

	if (enemy)
		enemy->SpawnEnemyEffectWeapon();
}