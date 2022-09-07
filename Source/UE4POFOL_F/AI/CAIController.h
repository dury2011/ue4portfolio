/* MEMO: CAIController.h 코드 배치 정리 완료  */
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "CAIController.generated.h"

UCLASS()
class UE4POFOL_F_API ACAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "AIController Setting")
	class UBehaviorTree* BehaviorTree;
	
	UPROPERTY(VisibleDefaultsOnly)
	class UAIPerceptionComponent* AIPerceptionComponent;

	class UAISenseConfig_Sight* Sight;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "AIController Setting")
	bool IsPlayerOnly = false;

private:	
	UPROPERTY()
	class ACPlayer* Player;

	UPROPERTY()
	class ACEnemy* Enemy;

	UPROPERTY()
	class ACCannon* Cannon;

	UPROPERTY()
	TArray<AActor*>Threats;
	
	UPROPERTY(EditDefaultsOnly, Category = "AIController Setting")
	bool InMission1 = false;

	UPROPERTY(EditDefaultsOnly, Category = "AIController Setting")
	bool InMission2 = false;

	UPROPERTY(EditDefaultsOnly, Category = "AIController Setting")
	bool InMission3 = false;

	UPROPERTY(EditDefaultsOnly, Category = "AIController Setting")
	bool IsBossController = false;

	FTimerHandle OpponentCheckTimer;
	//FTimerHandle DestroyCheckTimer;

	bool IsSkillActivate = false;
	
	bool IsDamaged = false;



protected:
	FGenericTeamId TeamId;

public:
	ACAIController(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	void OnPerception(AActor* Actor, FAIStimulus Stimulus);

	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

private:
	UFUNCTION()
	void StopEnemyAI();

	void CheckOpponent();
	//void CheckDestroy();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
};
