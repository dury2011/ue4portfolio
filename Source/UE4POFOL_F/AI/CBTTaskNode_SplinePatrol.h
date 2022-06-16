#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CBTTaskNode_SplinePatrol.generated.h"

UCLASS()
class UE4POFOL_F_API UCBTTaskNode_SplinePatrol : public UBTTaskNode
{
	GENERATED_BODY()
	
private:
	int32 Index;

public:
	UCBTTaskNode_SplinePatrol();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;	
};
