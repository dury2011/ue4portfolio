#include "Notify/CAnimNotify_SetAttackBySkill.h"
#include "Global.h"
#include "Enemy/CEnemy.h"

// TODO: 이 노티파이 삭제 예정
FString UCAnimNotify_SetAttackBySkill::GetNotifyName_Implementation() const
{
	return FString("Enemy Damage by Skill End?");
}

void UCAnimNotify_SetAttackBySkill::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACEnemy* enemy = Cast<ACEnemy>(MeshComp->GetOwner());

	if (enemy)
		enemy->SetIsAttackByPlayer(!IsEnemyEndDamageBySkill);
}

