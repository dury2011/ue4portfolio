/* MEMO: CEnemy.h 코드 배치 정리 완료 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapon/CWeaponStructure.h"
#include "GenericTeamAgentInterface.h"
#include "Interface/IRifle.h"
#include "CEnemy.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnEnemyMontageEnded);

UCLASS()
class UE4POFOL_F_API ACEnemy : public ACharacter, public IIRifle, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Camera Class")
	TSubclassOf<class UMatineeCameraShake> CameraShakeClass;

	UPROPERTY()
	TSubclassOf<class UAnimInstance> AnimInstance;	

	FOnEnemyMontageEnded OnEnemyMontageEnded;
	class ACAIController* Controller;	

private:
	struct FDamaged
	{
		float DamageAmount;
		FActionDamageEvent* DamageEvent;
		class AController* EventInstigator;
		class AActor* DamageCauser;
	} Damaged;

	// Player 형 변환을 위한 Character
	UPROPERTY()
	ACharacter* Character;

	UPROPERTY()
	ACharacter* Opponent;

	UPROPERTY()
	class UBlackboardComponent* Blackboard;

	UPROPERTY(EditDefaultsOnly, Category = "Rifle Class")
	TSubclassOf<class ACRifle> RifleClass;

	UPROPERTY()
	class ACRifle* Rifle;

	class UMaterialInstanceDynamic* Materials[2];

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HealthBarDisplayTime;

	FTimerHandle HealthBarTimer;

	FTimerHandle TakeDamageIntervalTimerHandle;

	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TMap<UUserWidget*, FVector> HitNumbers;

	UPROPERTY(EditAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HitNumberDestroyTime;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly, Category = "Team")
	uint8 TeamId = 2;

	FVector Pos;

public:
	ACEnemy();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

private:
	void DoAction();
	void Damage();
	void Dead();

public:
	void OnUnarmedMode();

public:
	UFUNCTION()
	void MontageEnded(UAnimMontage* InMontage, bool Ininterruped);

private:
	void ShakeCamera(FDamaged damage);

protected:
	UFUNCTION(BlueprintCallable)
	void StoreHitNumber(UUserWidget* InHitNumber, FVector InLocation);

	UFUNCTION()
	void DestroyHitNumber(UUserWidget* InHitNumber);

	UFUNCTION()
	void UpdateHitNumbers();

	UFUNCTION(BlueprintNativeEvent)
	void ShowHealthBar();
	void ShowHealthBar_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
	void HideHealthBar();

public:
	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitNumber(int32 InDamage, FVector InHitLocation);

	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(TeamId); }
	
	FORCEINLINE class UBehaviorTree* GetBehaviorTree() { return BehaviorTree; }
	FORCEINLINE class ACRifle* GetRifle() override { return Rifle; }
	FORCEINLINE void SetBlackboard(class UBlackboardComponent* InBlackboard) { Blackboard = InBlackboard; }
};