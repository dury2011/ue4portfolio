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
	EBTNodeResult::Type result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto character = Cast<ACEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	
	if (character == nullptr)
		return EBTNodeResult::Failed;

	character->OnAttack();

	IsAttacking = true;
	character->OnEnemyAttackEnded.AddLambda([this]() -> void { IsAttacking = false; });

	FTimerHandle forceFinishTimer;

	GetWorld()->GetTimerManager().SetTimer(forceFinishTimer, FTimerDelegate::CreateLambda([&]()
	{
		IsAttacking = false;
	}), 3.5f, false, 3.5f);
	
	return EBTNodeResult::InProgress;
}

void UCBTTaskNode_EnemyAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!IsAttacking)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}