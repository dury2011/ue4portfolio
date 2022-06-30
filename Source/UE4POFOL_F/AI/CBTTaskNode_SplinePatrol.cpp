#include "AI/CBTTaskNode_SplinePatrol.h"
#include "Global.h"
#include "AI/CAIController.h"
#include "Enemy/CEnemy_Rifle.h"
#include "Object/CSpline.h"

UCBTTaskNode_SplinePatrol::UCBTTaskNode_SplinePatrol()
{
	bNotifyTick = true;

	NodeName = "Spline Patrol";
}

EBTNodeResult::Type UCBTTaskNode_SplinePatrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//Super::ExecuteTask(OwnerComp, NodeMemory);

	//ACAIController* controller = Cast<ACAIController>(OwnerComp.GetOwner());
	//ACEnemy_Rifle* enemyRifle = Cast<ACEnemy_Rifle>(controller->GetPawn());

	//FVector location;
	////float distance;

	////if (enemyRifle->GetSpline()->GetMoveToSplinePoint(location, distance) == false)
	////{
	////	enemyRifle->Destroy();
	////
	////	return EBTNodeResult::Failed;
	////}

	return EBTNodeResult::InProgress;
}

void UCBTTaskNode_SplinePatrol::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	//Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	//ACAIController* controller = Cast<ACAIController>(OwnerComp.GetOwner());
	//ACEnemy_Rifle* enemyRifle = Cast<ACEnemy_Rifle>(controller->GetPawn());

	//FVector location;
	//float distance;

	////enemyRifle->GetSpline()->GetMoveToSplinePoint(location, distance);
	//EPathFollowingRequestResult::Type type = controller->MoveToLocation(location, distance, false);

	//if (type == EPathFollowingRequestResult::Failed)
	//{
	//	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	//	return;
	//}

	//if (type == EPathFollowingRequestResult::AlreadyAtGoal)
	//{
	//	//enemyRifle->GetSpline()->SetUpdateMoveToSplinePoint();
	//	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	//	return;
	//}
}