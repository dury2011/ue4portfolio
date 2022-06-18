#include "AI/CBTTaskNode_EnemyHelixAttack.h"
#include "Global.h"
#include "AI/CAIController.h"
#include "Enemy/CEnemy_Helix.h"
#include "Component/CCharacterComponent.h"

UCBTTaskNode_EnemyHelixAttack::UCBTTaskNode_EnemyHelixAttack()
{
	bNotifyTick = true;

	NodeName = "Helix Attack";
}

EBTNodeResult::Type UCBTTaskNode_EnemyHelixAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ACAIController* controller = Cast<ACAIController>(OwnerComp.GetOwner());
	EnemyHelix = Cast<ACEnemy_Helix>(controller->GetPawn());

	if (EnemyHelix)
	{
		EnemyHelix->OnAttack();

		return EBTNodeResult::InProgress;
	}
	else
		return EBTNodeResult::Failed;
}

void UCBTTaskNode_EnemyHelixAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!EnemyHelix->GetCharacterComponent()->GetIsMontagePlaying())
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
