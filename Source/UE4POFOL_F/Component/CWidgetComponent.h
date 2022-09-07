#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CWidgetComponent.generated.h"

UENUM(BlueprintType)
enum class EWarnningTextType : uint8
{
	LowHp, LowMp, LowMp_Skill, CoolTime_Skill, Max
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE4POFOL_F_API UCWidgetComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	ACharacter* OwnerCharacter;

	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicMaterial;
	
	class ICInterface_PlayerState* PlayerInterface;

	float CurrentHp;
	float CurrentMp;
	float CurrentSp;

	float MaxHp;
	float MaxMp;
	float MaxSp;

	FHitResult HitResult;

	bool IsEventCalled = false;
	bool bCrosshairVisible = true;

public:	
	UCWidgetComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	void CreateLineTrace();

public:
	UFUNCTION(BlueprintCallable)
	bool CanCrosshairWidgetColorChange();

	FORCEINLINE void SetbCrosshairVisible(bool Inbool) { bCrosshairVisible = Inbool; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetbCrosshairVisible() { return bCrosshairVisible; }

	FORCEINLINE FHitResult GetHitResult() { return HitResult; }

	//UFUNCTION(BlueprintCallable)
	//float GetCurrentHpPercentage();

	//UFUNCTION(BlueprintCallable)
	//float GetCurrentMpPercentage();

	//void CheckWarnningText();

	//UFUNCTION(BlueprintImplementableEvent)
	//void WarnningText_LowMp();

	//UFUNCTION(BlueprintImplementableEvent)
	//void WarnningText_LowHp();

	//UFUNCTION(BlueprintImplementableEvent)
	//void WarnningText_CannotUseSkill_LowMp();

	//UFUNCTION(BlueprintImplementableEvent)
	//void WarnningText_CannotUseSkill_CoolTime();
};
