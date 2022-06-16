/* MEMO: CAIController.h 코드 배치 정리 완료  */
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CAIController.generated.h"

UCLASS()
class UE4POFOL_F_API ACAIController : public AAIController
{
	GENERATED_BODY()
	
	//UPROPERTY(VisibleDefaultsOnly)
	//class UAIPerceptionComponent* Perception;
	//
	UPROPERTY()
	class ACPlayer* Player;

	UPROPERTY()
	class ACEnemy* Enemy;
	//UPROPERTY()
	//class UAISenseConfig_Sight* Sight;

	//UPROPERTY()
	//class UBlackboardData* Blackboard;
private:
	FTimerHandle Timer;

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "AIController Setting")
	class UBehaviorTree* BehaviorTree;

public:
	ACAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:
	//UFUNCTION()
	//void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	void OnRepeatTimer();
};
