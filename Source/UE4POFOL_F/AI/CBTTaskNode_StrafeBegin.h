#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CBTTaskNode_StrafeBegin.generated.h"

UCLASS()
class UE4POFOL_F_API UCBTTaskNode_StrafeBegin : public UBTTaskNode
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "Setting")
	float StrafingTime = 1.5f;

	bool bExecute = true;
	FTimerHandle StopTimer;
private:
	UCBTTaskNode_StrafeBegin();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)override;

	void StopStrafing();
};
