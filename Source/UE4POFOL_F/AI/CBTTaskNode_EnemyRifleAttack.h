#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CBTTaskNode_EnemyRifleAttack.generated.h"

UCLASS()
class UE4POFOL_F_API UCBTTaskNode_EnemyRifleAttack : public UBTTaskNode
{
	GENERATED_BODY()

private:
	UPROPERTY()
	class ACEnemy_Rifle* EnemyRifle;
	
private:
	UCBTTaskNode_EnemyRifleAttack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)override;
};
