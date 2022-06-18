#include "Notify/CAnimNotify_SpawnHelixProjectile.h"
#include "Global.h"
#include "Enemy/CEnemy_Helix.h"

FString UCAnimNotify_SpawnHelixProjectile::GetNotifyName_Implementation() const
{
	return "SpawnSpellProjectile_R";
}

void UCAnimNotify_SpawnHelixProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACEnemy_Helix* enemyHelix = Cast<ACEnemy_Helix>(MeshComp->GetOwner());

	if (enemyHelix)
		enemyHelix->SpawnHelixProjectile();
}