/* MEMO: CBTService_Enemy.h 코드 배치 정리 완료 */
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "CBTService_Enemy.generated.h"

UCLASS()
class UE4POFOL_F_API UCBTService_Enemy : public UBTService
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "Action")
	float AttackRangeOnehand = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Action Rifle Idle")
	float AttackRangeRifle = 3000.0f;

public:
	UCBTService_Enemy();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
