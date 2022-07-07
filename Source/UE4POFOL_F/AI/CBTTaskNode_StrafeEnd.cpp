#include "AI/CBTTaskNode_StrafeEnd.h"
#include "Global.h"
#include "Enemy/CEnemy.h"
#include "AI/CAIController.h"

UCBTTaskNode_StrafeEnd::UCBTTaskNode_StrafeEnd()
{
	bNotifyTick = false;

	NodeName = "End Strafe";
}

EBTNodeResult::Type UCBTTaskNode_StrafeEnd::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto character = Cast<ACEnemy>(OwnerComp.GetAIOwner()->GetPawn());

	if (character == nullptr)
		return EBTNodeResult::Failed;

	character->EndStrafing();

	return EBTNodeResult::Succeeded;
}

