#include "Notify/CAnimNotify_EnemyRifleProjectile.h"
#include "Global.h"
#include "Enemy/CEnemy_Rifle.h"

FString UCAnimNotify_EnemyRifleProjectile::GetNotifyName_Implementation() const
{
	return "Spawn Enemy Rifle Projectile";
}

void UCAnimNotify_EnemyRifleProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACEnemy_Rifle* enemyRifle = Cast<ACEnemy_Rifle>(MeshComp->GetOwner());

	if (enemyRifle)
		enemyRifle->SpawnAndShootProjectile();
}