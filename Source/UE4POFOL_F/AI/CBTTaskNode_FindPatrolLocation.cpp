#include "AI/CBTTaskNode_FindPatrolLocation.h"
#include "Global.h"
#include "AI/CAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UCBTTaskNode_FindPatrolLocation::UCBTTaskNode_FindPatrolLocation()
{
	NodeName = TEXT("Find Patrol Location");
}

EBTNodeResult::Type UCBTTaskNode_FindPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto controllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (controllingPawn == nullptr)
		return EBTNodeResult::Failed;

	UNavigationSystemV1* navSystem = UNavigationSystemV1::GetNavigationSystem(controllingPawn->GetWorld());
	
	if (navSystem == nullptr)
		return EBTNodeResult::Failed;

	FVector origin = OwnerComp.GetBlackboardComponent()->GetValueAsVector("SpawnLocation");
	FNavLocation nextPatrol;
	
	if (navSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 500.0f, nextPatrol))
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector("PatrolLocation", nextPatrol.Location);
		
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}