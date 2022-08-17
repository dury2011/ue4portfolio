#include "Notify/CAnimNotify_SpawnEnemy.h"
#include "Global.h"
#include "Enemy/CEnemy.h"

FString UCAnimNotify_SpawnEnemy::GetNotifyName_Implementation() const
{
	return FString("Spawn Enemy");
}

void UCAnimNotify_SpawnEnemy::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	CheckNull(MeshComp);
	
	ACEnemy* spawnEnemy = ACEnemy::SpawnEnemy(MeshComp->GetOwner(), SpawnEnemyClass, SpawnSocketName);

	//if (spawnEnemy)
	//{
	//	spawnEnemy->SetSpawnCount();

	//	if (spawnEnemy->GetSpawnCount() > 9)
	//		spawnEnemy->DestroyEnemy();
	//}
}
