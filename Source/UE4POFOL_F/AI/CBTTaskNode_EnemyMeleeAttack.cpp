#include "AI/CBTTaskNode_EnemyMeleeAttack.h"
#include "Global.h"
#include "AI/CAIController.h"
#include "Enemy/CEnemy_Meele.h"
#include "Component/CCharacterComponent.h"

UCBTTaskNode_EnemyMeleeAttack::UCBTTaskNode_EnemyMeleeAttack()
{
	bNotifyTick = true;

	NodeName = "Attack Melee";
}

EBTNodeResult::Type UCBTTaskNode_EnemyMeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ACAIController* controller = Cast<ACAIController>(OwnerComp.GetOwner());
	EnemyMelee = Cast<ACEnemy_Meele>(controller->GetPawn());

	if (EnemyMelee)
	{
		EnemyMelee->OnAttack();

		return EBTNodeResult::InProgress;
	}
	else
		return EBTNodeResult::Failed;
}

void UCBTTaskNode_EnemyMeleeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!EnemyMelee->GetCharacterComponent()->GetIsMontagePlaying())
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}