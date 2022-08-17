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
	FVector current = controllingPawn->GetActorLocation();

	

	if (navSystem->GetRandomPointInNavigableRadius(current, UKismetMathLibrary::RandomFloatInRange(450.0f, 750.0f), nextPatrol))
	{
		current = FVector(nextPatrol.Location.X, nextPatrol.Location.Y, nextPatrol.Location.Z) + GraduallyTowardsWorldLoc;
		
		OwnerComp.GetBlackboardComponent()->SetValueAsVector("PatrolLocation", current);
		
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}