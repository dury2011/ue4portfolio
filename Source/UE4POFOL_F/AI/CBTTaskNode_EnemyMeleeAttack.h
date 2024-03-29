#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CBTTaskNode_EnemyMeleeAttack.generated.h"

UCLASS()
class UE4POFOL_F_API UCBTTaskNode_EnemyMeleeAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
	private:
	UPROPERTY()
	class ACEnemy_Meele* EnemyMelee;

private:
	UCBTTaskNode_EnemyMeleeAttack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)override;
	
};
