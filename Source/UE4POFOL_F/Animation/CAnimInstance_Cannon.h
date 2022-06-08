#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Player/CCannon.h"
#include "CAnimInstance_Cannon.generated.h"

UCLASS()
class UE4POFOL_F_API UCAnimInstance_Cannon : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float Pitch;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float Roll;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	float Yaw;

	UPROPERTY(BlueprintReadWrite, Category = "Animation")
	ECannonStateType CurrentCannonStateType;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	ECannonStateType PreviousCannonStateType;

private:
	UPROPERTY()
	class ACCannon* Cannon;

private:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
	void CannonStateTypeChange(ECannonStateType InPreviousType, ECannonStateType InCurrentType);
};
