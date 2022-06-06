/* MEMO: CAIController.h 코드 배치 정리 완료  */
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CAIController.generated.h"

UCLASS()
class UE4POFOL_F_API ACAIController : public AAIController
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleDefaultsOnly)
	class UAIPerceptionComponent* Perception;

	class ACEnemy* Enemy;
	class UAISenseConfig_Sight* Sight;
	//class UCAIStateComponent* AIStateComponent;

public:
	ACAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:
	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);
};
