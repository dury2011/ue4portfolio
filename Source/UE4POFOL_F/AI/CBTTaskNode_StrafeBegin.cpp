#include "AI/CBTTaskNode_StrafeBegin.h"
#include "Global.h"
#include "Enemy/CEnemy.h"
#include "AI/CAIController.h"

UCBTTaskNode_StrafeBegin::UCBTTaskNode_StrafeBegin()
{
	//bNotifyTick = true;

	NodeName = "Begin Strafe";
}

EBTNodeResult::Type UCBTTaskNode_StrafeBegin::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type result = Super::ExecuteTask(OwnerComp, NodeMemory);

	Enemy = Cast<ACEnemy>(OwnerComp.GetAIOwner()->GetPawn());

	if (Enemy == nullptr)
		return EBTNodeResult::Failed;
	
	Enemy->GetWorldTimerManager().SetTimer(EndTimer, this, &UCBTTaskNode_StrafeBegin::EndStrafing, StrafingTime, false, StrafingTime);
	Enemy->GetWorldTimerManager().SetTimer(ChangeDirectionTimer, this, &UCBTTaskNode_StrafeBegin::ChangeStrafing, ChangeDirectionTime, false, ChangeDirectionTime);
	Enemy->BeginStrafing();

	bNotifyTick = true;

	return EBTNodeResult::InProgress;
}

void UCBTTaskNode_StrafeBegin::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (bFinish)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UCBTTaskNode_StrafeBegin::ChangeStrafing()
{
	Enemy->ChangeStrafing();
	
	Enemy->GetWorldTimerManager().ClearTimer(ChangeDirectionTimer);
}

void UCBTTaskNode_StrafeBegin::EndStrafing()
{
	Enemy->EndStrafing();

	bFinish = true;

	Enemy->GetWorldTimerManager().ClearTimer(EndTimer);
}


