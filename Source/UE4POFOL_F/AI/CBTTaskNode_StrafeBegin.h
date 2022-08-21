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
	float StrafingTime = 2.0f;
	
	UPROPERTY(EditAnywhere, Category = "Setting")
	float ChangeDirectionTime = 1.0f;
	
	UPROPERTY()
	class ACEnemy* Enemy;
	
	FTimerHandle EndTimer;
	FTimerHandle ChangeDirectionTimer;

	EEnemyStrafingType CurrentStrafingType;

	FVector StrafeDirection = FVector::ZeroVector;

	bool bFinish = false;

private:
	UCBTTaskNode_StrafeBegin();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)override;
	//void ChangeStrafing();
	//void EndStrafing();
};
