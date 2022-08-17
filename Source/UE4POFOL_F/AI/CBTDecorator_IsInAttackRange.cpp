#include "AI/CBTDecorator_IsInAttackRange.h"
#include "Global.h"
#include "AI/CAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

UCBTDecorator_IsInAttackRange::UCBTDecorator_IsInAttackRange()
{
	NodeName = TEXT("Can Attack");
}

bool UCBTDecorator_IsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);

	auto controllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (controllingPawn == nullptr)
		return false;

	auto target = Cast<ACharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("Opponent"));

	if (target == nullptr)
		return false;

	bResult = (target->GetDistanceTo(controllingPawn) <= AttackRange);
		return bResult;
}