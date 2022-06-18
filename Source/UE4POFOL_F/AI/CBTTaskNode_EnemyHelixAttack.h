#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CBTTaskNode_EnemyHelixAttack.generated.h"

UCLASS()
class UE4POFOL_F_API UCBTTaskNode_EnemyHelixAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	class ACEnemy_Helix* EnemyHelix;

private:
	UCBTTaskNode_EnemyHelixAttack();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)override;
};
