/* MEMO : ACAIController_Boss.h 코드 배치 정리 완료 */
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CAIController_Boss.generated.h"

UCLASS()
class UE4POFOL_F_API ACAIController_Boss : public AAIController
{
	GENERATED_BODY()
	
public:

private:
	UPROPERTY()
	class ACBoss* Boss;

	UPROPERTY(VisibleDefaultsOnly)
	class UAIPerceptionComponent* Perception_Sight;

	UPROPERTY()
	class UAISenseConfig_Sight* Sight;

public:
	ACAIController_Boss();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:
	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);
};
