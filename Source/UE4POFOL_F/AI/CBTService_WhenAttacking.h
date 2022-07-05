#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "CBTService_WhenAttacking.generated.h"

UCLASS()
class UE4POFOL_F_API UCBTService_WhenAttacking : public UBTService
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "Setting")
	float OpponentDetectRadius = 300.0f;
	
public:
	UCBTService_WhenAttacking();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
