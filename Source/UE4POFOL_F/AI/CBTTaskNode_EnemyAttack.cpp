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

	Character = Cast<ACEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	
	if (Character == nullptr)
		return EBTNodeResult::Failed;

	Character->OnAttack();

	IsAttacking = true;
	Character->OnEnemyAttackEnded.AddLambda([this]() -> void { IsAttacking = false; });

	FTimerHandle forceFinishTimer;

	GetWorld()->GetTimerManager().SetTimer(forceFinishTimer, FTimerDelegate::CreateLambda([&]()
	{
		IsAttacking = false;
	}), 30.0f, false, 30.0f);
	
	return EBTNodeResult::InProgress;
}

void UCBTTaskNode_EnemyAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!IsAttacking)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	if (Character->GetCurrentEnemyStateType() == EEnemyStateType::Dead)
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

	//if (Character->IsInterrupted == true)
	//{
	//	Character->IsInterrupted = false;
	//	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	//}
}