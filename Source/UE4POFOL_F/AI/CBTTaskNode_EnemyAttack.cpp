#include "AI/CBTTaskNode_EnemyAttack.h"
#include "Global.h"
#include "Enemy/CEnemy.h"
#include "AI/CAIController.h"

UCBTTaskNode_EnemyAttack::UCBTTaskNode_EnemyAttack()
{
	bNotifyTick = true;

	NodeName = "Attack";
}

EBTNodeResult::Type UCBTTaskNode_EnemyAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ACAIController* controller = Cast<ACAIController>(OwnerComp.GetOwner());
	Enemy = Cast<ACEnemy>(controller->GetPawn());
	
	if (Enemy)
	{
		Enemy->OnAttack();
	
		return EBTNodeResult::InProgress;
	}
	else
		return EBTNodeResult::Failed;
}

void UCBTTaskNode_EnemyAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!Enemy->GetbMontageIsPlaying())
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}