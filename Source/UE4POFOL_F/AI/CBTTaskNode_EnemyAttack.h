#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CBTTaskNode_EnemyAttack.generated.h"

UCLASS()
class UE4POFOL_F_API UCBTTaskNode_EnemyAttack : public UBTTaskNode
{
	GENERATED_BODY()

private:
	bool IsAttacking = false;

	UPROPERTY()
	class ACEnemy* Character;

private:
	UCBTTaskNode_EnemyAttack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)override;
	
};
