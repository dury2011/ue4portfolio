#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CBTTaskNode_StrafeEnd.generated.h"

UCLASS()
class UE4POFOL_F_API UCBTTaskNode_StrafeEnd : public UBTTaskNode
{
	GENERATED_BODY()
	
private:
	UCBTTaskNode_StrafeEnd();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
};
