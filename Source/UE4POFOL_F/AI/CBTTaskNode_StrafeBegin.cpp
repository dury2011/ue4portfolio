#include "AI/CBTTaskNode_StrafeBegin.h"
#include "Global.h"
#include "Enemy/CEnemy.h"
#include "AI/CAIController.h"

UCBTTaskNode_StrafeBegin::UCBTTaskNode_StrafeBegin()
{
	bNotifyTick = true;

	NodeName = "Begin Strafe";
}

EBTNodeResult::Type UCBTTaskNode_StrafeBegin::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto character = Cast<ACEnemy>(OwnerComp.GetAIOwner()->GetPawn());

	if (character == nullptr)
		return EBTNodeResult::Failed;
	else
	{
		bExecute = true;
		
		character->BeginStrafing();
		character->GetWorldTimerManager().SetTimer(StopTimer, this, &UCBTTaskNode_StrafeBegin::StopStrafing, StrafingTime);
	}

	return EBTNodeResult::InProgress;
}

void UCBTTaskNode_StrafeBegin::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	auto character = Cast<ACEnemy>(OwnerComp.GetAIOwner()->GetPawn());

	if (character == nullptr)
		return;

	if (!bExecute)
	{
		character->GetWorldTimerManager().ClearTimer(StopTimer);
		character->EndStrafing();
		
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UCBTTaskNode_StrafeBegin::StopStrafing()
{
	bExecute = false;
}
