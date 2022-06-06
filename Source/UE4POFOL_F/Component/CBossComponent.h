/* MEMO: CBossActorComponent.h 코드 배치 정리 완료 */
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CBossComponent.generated.h"

UENUM(BlueprintType)
enum class EBossStateType : uint8
{
	Idle, AttackNormal, AttackShield, AttackJump, AttackRocket, AttackBallEight, AttackCallFriend, Max
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBossStateTypeChanged, EBossStateType, currentType, EBossStateType, prevType);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE4POFOL_F_API UCBossComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FBossStateTypeChanged OnBossStateTypeChanged;
	
	UPROPERTY(EditDefaultsOnly, Category = "Data Asset")
	class UCDataAsset_Boss* DataAsset;
	
	UPROPERTY(EditDefaultsOnly, Category = "State")
	float Hp = 2000.0f;
private:
	
	UPROPERTY()
	class ACharacter* OwnerCharacter;
	
	float FullHp = 2000.0f;
	
	EBossStateType CurrentType;

public:	
	UCBossComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void DoAction();
	
	UFUNCTION(BlueprintCallable, Category = "Hp")
	void SetHp(float InHp);

	UFUNCTION(BlueprintCallable, Category = "Hp")
	void AddHp(float InHp);

	UFUNCTION(BlueprintCallable, Category = "Hp")
	void SubHp(float InHp);

	UFUNCTION(BlueprintCallable, Category = "Hp")
	float GetHpPercentage();

	void SetIdleMode();
	void SetAttackNormalMode();
	void SetAttackShieldMode();
	void SetAttackJumpMode();
	void SetAttackRocketMode();
	void SetAttackBallEightMode();
	void SetAttackCallFriendMode();

	void ChangeType(EBossStateType InCurrentType);
	
	UFUNCTION(BlueprintCallable, Category = "Hp")
	FORCEINLINE float GetHp() { return Hp; }

	FORCEINLINE UCDataAsset_Boss* GetDataAsset() { return DataAsset; }
	FORCEINLINE EBossStateType GetCurrentType() { return CurrentType; }
	FORCEINLINE bool IsIdleMode() { return CurrentType == EBossStateType::Idle; }
	FORCEINLINE bool IsAttackNormalMode() { return CurrentType == EBossStateType::AttackNormal; }
	FORCEINLINE bool IsAttackShieldMode() { return CurrentType == EBossStateType::AttackShield; }
	FORCEINLINE bool IsAttackJumpMode() { return CurrentType == EBossStateType::AttackJump; }
	FORCEINLINE bool IsAttackRocketMode() { return CurrentType == EBossStateType::AttackRocket; }
	FORCEINLINE bool IsAttackBallEightMode() { return CurrentType == EBossStateType::AttackBallEight; }
	FORCEINLINE bool IsAttackCallFriendMode() { return CurrentType == EBossStateType::AttackCallFriend; }	
};
