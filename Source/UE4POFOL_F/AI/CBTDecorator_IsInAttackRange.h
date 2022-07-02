#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "CBTDecorator_IsInAttackRange.generated.h"

UCLASS()
class UE4POFOL_F_API UCBTDecorator_IsInAttackRange : public UBTDecorator
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, Category = "Setting")
	float AttackRange = 1500.0f;

public:
	UCBTDecorator_IsInAttackRange();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
