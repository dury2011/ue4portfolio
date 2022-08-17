#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CBTTaskNode_FindPatrolLocation.generated.h"

UCLASS()
class UE4POFOL_F_API UCBTTaskNode_FindPatrolLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere)
	FVector GraduallyTowardsWorldLoc = FVector::ZeroVector;
	
public:
	UCBTTaskNode_FindPatrolLocation();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
};
