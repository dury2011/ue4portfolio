#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Component/CBossComponent.h"
#include "CAnimInstance_Boss.generated.h"

UCLASS()
class UE4POFOL_F_API UCAnimInstance_Boss : public UAnimInstance
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "boss Movement")
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category = "Boss Movement")
	bool IsFalling;
	
	UPROPERTY(BlueprintReadOnly, Category = "BossStateType")
	EBossStateType CurrentBossStateType = EBossStateType::Max;

	UPROPERTY(BlueprintReadOnly, Category = "BossStateType")
	EBossStateType PreviousBossStateType = EBossStateType::Max;

private:
	class ACharacter* OwnerCharacter;
	class UCBossComponent* BossComponent;

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
	void BossStateTypeChanged(EBossStateType InCurrentType, EBossStateType InPrevType);
};
