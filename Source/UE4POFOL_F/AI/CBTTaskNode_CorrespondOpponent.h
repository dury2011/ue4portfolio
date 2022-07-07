#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "CBTTaskNode_CorrespondOpponent.generated.h"

UCLASS()
class UE4POFOL_F_API UCBTTaskNode_CorrespondOpponent : public UBTTaskNode
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "Setting")
	float StrafingTime = 1.0f; 

	//FTimerHandle StrafeTimer;

	bool IsAttacking = false;
	bool IsDodging = false;
	bool IsDefencing = false;
	bool IsSwitchToAttack = false;
	bool IsSwitchToDodge = false;
	bool IsSwitchToDefence = false;

private:
	UCBTTaskNode_CorrespondOpponent();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)override;
	void EndStrafing();
};
