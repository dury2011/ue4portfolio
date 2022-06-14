#include "AI/CBTTaskNode_EnemyRifleAttack.h"
#include "Global.h"
#include "AI/CAIController.h"
#include "Enemy/CEnemy_Rifle.h"
#include "Component/CCharacterComponent.h"

UCBTTaskNode_EnemyRifleAttack::UCBTTaskNode_EnemyRifleAttack()
{
	bNotifyTick = true;

	NodeName = "Attack Rifle";
}

EBTNodeResult::Type UCBTTaskNode_EnemyRifleAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ACAIController* controller = Cast<ACAIController>(OwnerComp.GetOwner());
	EnemyRifle = Cast<ACEnemy_Rifle>(controller->GetPawn());

	if (EnemyRifle)
	{
		EnemyRifle->OnFire();

		return EBTNodeResult::InProgress;
	}
	else
		return EBTNodeResult::Failed;
}

void UCBTTaskNode_EnemyRifleAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!EnemyRifle->GetCharacterComponent()->GetIsMontagePlaying())
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}