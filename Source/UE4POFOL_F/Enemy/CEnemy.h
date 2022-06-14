/* MEMO: CEnemy.h 코드 배치 정리 완료 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapon/CWeaponStructure.h"
#include "GenericTeamAgentInterface.h"
#include "Interface/IRifle.h"
#include "CEnemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyStateType : uint8
{
	Idle, Walk, Run, Parkour, Attack, Damage, Dead, Max
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyStateTypeChanged, EEnemyStateType, InPreviousType, EEnemyStateType, InCurrentType);
DECLARE_MULTICAST_DELEGATE(FOnEnemyMontageEnded);

UCLASS()
class UE4POFOL_F_API ACEnemy : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnEnemyStateTypeChanged OnEnemyStateTypeChanged;

protected:	
	UPROPERTY()
	TSubclassOf<class UAnimInstance> AnimInstance;	

	UPROPERTY(VisibleDefaultsOnly)
	class UCCharacterComponent* CharacterComponent;
	//class ACAIController* PossessedController;	
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Enemy Setting")
	TSubclassOf<class UMatineeCameraShake> CameraShakeClass;
	
	FOnEnemyMontageEnded OnEnemyMontageEnded;

private:
	//// Player 형 변환을 위한 Character
	//UPROPERTY()
	//ACharacter* Character;
	
	UPROPERTY()
	TArray<class UCapsuleComponent*> CapsuleCollisions;
	
	UPROPERTY()
	class ACharacter* Opponent;

	UPROPERTY()
	class UBlackboardComponent* Blackboard;

	//UPROPERTY(EditDefaultsOnly, Category = "Rifle Class")
	//TSubclassOf<class ACRifle> RifleClass;

	//UPROPERTY()
	//class ACRifle* Rifle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Enemy Setting")
	float HealthBarDisplayTime;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Enemy Setting")
	TMap<UUserWidget*, FVector> HitNumbers;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Enemy Setting")
	float HitNumberDestroyTime;

	//UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	//class UBehaviorTree* BehaviorTree;
	
	EEnemyStateType CurrentStateType = EEnemyStateType::Max;

	FTimerHandle HealthBarTimer;

	struct FDamaged
	{
		float DamageAmount;
		FActionDamageEvent* DamageEvent;
		class AController* EventInstigator;
		class AActor* DamageCauser;
	} Damaged;

	bool bDamage;

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
	
	//FORCEINLINE class UBehaviorTree* GetBehaviorTree() { return BehaviorTree; }

	//FORCEINLINE UBlackboardComponent* GetBlackboard() { return Blackboard; }
	//FORCEINLINE void SetBlackboard(class UBlackboardComponent* InBlackboard) { Blackboard = InBlackboard; }

protected:

	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* InMontage, bool InInterrupted);

public:
	void OnStateTypeChange(EEnemyStateType InCurrentStateType);
	FORCEINLINE bool GetbDamage() { return bDamage; }
	FORCEINLINE ACharacter* GetOpponent() { return Opponent; }
	FORCEINLINE UCCharacterComponent* GetCharacterComponent() { return CharacterComponent; }
	FORCEINLINE EEnemyStateType GetCurrentEnemyStateType() { return CurrentStateType; }
};