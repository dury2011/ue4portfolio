#include "AI/CBTTaskNode_CorrespondOpponent.h"
#include "Global.h"
#include "Enemy/CEnemy.h"
#include "AI/CAIController.h"

UCBTTaskNode_CorrespondOpponent::UCBTTaskNode_CorrespondOpponent()
{
	bNotifyTick = true;

	NodeName = "Correspond Opponent";
}

EBTNodeResult::Type UCBTTaskNode_CorrespondOpponent::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto character = Cast<ACEnemy>(OwnerComp.GetAIOwner()->GetPawn());

	if (character == nullptr)
		return EBTNodeResult::Failed;

	int32 rand = UKismetMathLibrary::RandomIntegerInRange(0, 1/*2*/);

	switch (rand)
	{
		case 0:
		{
			//IsSwitchToAttack = true;
			//character->OnAttack();
			//IsAttacking = true;
			//character->OnEnemyAttackEnded.AddLambda([this]() -> void { IsAttacking = false; });
			
			break;
		}
		case 1:
		{
			//TODO: 다른것 구현
			IsSwitchToDodge = true;
			character->BeginDodge();
			IsDodging = true;
			character->OnEnemyParkourEnded.AddLambda([this]() -> void {IsDodging = false; });

			break;
		}
		//case 2:
		//{
			// TODO: 추후 구현
			//character->OnShield();
			//GLog->Log("UCBTTaskNode_CorrespondOpponent::Enemy Shield Activated!");
		//}
		default:
			break;	
	}

	return EBTNodeResult::InProgress;
}

void UCBTTaskNode_CorrespondOpponent::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (IsSwitchToDodge && !IsDodging)
	{
		IsSwitchToDodge = false;
		
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

		return;
	}
	else if (IsSwitchToAttack && !IsAttacking)
	{
		IsSwitchToAttack = false;
		
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);

		return;
	}
}
