/* MEMO: CEnemy.h 코드 배치 정리 완료 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapon/CWeaponStructure.h"
#include "GenericTeamAgentInterface.h"
#include "CEnemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyStateType : uint8
{
	IdleOrJustMoving, Parkour, Attack, Damage, Dead, Max
};

UENUM(BlueprintType)
enum class EEnemyStrafingType : uint8
{
	Front = 0, Back, Left, Right, Max
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyStateTypeChanged, EEnemyStateType, InPreviousType, EEnemyStateType, InCurrentType);
DECLARE_MULTICAST_DELEGATE(FOnEnemyMontageEnded);
DECLARE_MULTICAST_DELEGATE(FOnEnemyAttackEnded);
DECLARE_MULTICAST_DELEGATE(FOnEnemyParkourEnded);
DECLARE_MULTICAST_DELEGATE(FOnEnemyMontageInterrupted);

UCLASS()
class UE4POFOL_F_API ACEnemy : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	FOnEnemyStateTypeChanged OnEnemyStateTypeChanged;

	FOnEnemyAttackEnded OnEnemyAttackEnded;

	FOnEnemyMontageEnded OnEnemyMontageEnded;

	FOnEnemyMontageInterrupted OnEnemyMontageInterrupted;

	FOnEnemyParkourEnded OnEnemyParkourEnded;

protected:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Setting")
	float Hp = 1000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Setting")
	float MaxHp = 1000.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Setting")
	float Mp = 1000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Setting")
	float MaxMp = 1000.0f;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Enemy Setting")
	float RotationSpeed = 3.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TArray<FActionData> ActionDatas;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TArray<FDamageData> DamageDatas;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TArray<FDamageData> DeadDatas;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TArray<FActionData> DodgeDatas;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TArray<TSubclassOf<class ACWeapon>> WeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TArray<TSubclassOf<class ACWeapon>> EffectWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TArray<TSubclassOf<class ACProjectile>> ProjectileClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TSubclassOf<class UMatineeCameraShake> DamageCameraShakeClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TSubclassOf<class UAnimInstance> AnimBlueprint;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Enemy Setting")
	class UWidgetComponent* HealthBarWidgetComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	float StopTime = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	float ChangeStrafingTypeInterval = 2.0f;

	bool bMontageIsPlaying = false;
	bool bActivateRotateToOpponent = true;
	bool CanStrafing = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Enemy Setting")
	FName EffectWeaponSpawnSocketName;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Enemy Setting")
	int32 EffectWeaponIndex;

private:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Enemy Setting")
	float HealthBarDisplayTime = 1.5f;
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Enemy Setting")
	float HitNumberDestroyTime = 1.0f;
	
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TMap<UUserWidget*, FVector> HitNumbers;
	
	UPROPERTY()
	TArray<class UCapsuleComponent*> CapsuleCollisions;
	
	UPROPERTY()
	class UBlackboardComponent* Blackboard;
	
	UPROPERTY()
	class ACharacter* Opponent;

	UPROPERTY()
	TArray<class ACWeapon*> Weapons;	
	
	FVector StrafeDirection = FVector::ZeroVector;
	EEnemyStateType CurrentStateType = EEnemyStateType::Max;
	EEnemyStateType PreviousStateType = EEnemyStateType::Max;
	FTimerHandle HealthBarTimer;
	FTimerHandle StopTimer;
	FTimerHandle StrafeTimer;

	EEnemyStrafingType CurrentStrafingType = EEnemyStrafingType::Max;
	EEnemyStrafingType PreviousEnemyStrafingType = EEnemyStrafingType::Max;
	float DistanceToOpponent;

	struct FDamaged
	{
		float DamageAmount;
		FActionDamageEvent* DamageEvent;
		class AController* EventInstigator;
		class AActor* DamageCauser;
	} Damaged;

public:
	ACEnemy();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

public:
	virtual void OnAttack();

private:
	void Damage();
	void Dead();

public:
	void BeginStrafing();
	void EndStrafing();
	void BeginDodge();
	void ChangeStrafing();

private:
	void RecoverDilation();
	void ShakeCamera(FDamaged damage);

protected:
	UFUNCTION(BlueprintCallable)
	void StoreHitNumber(UUserWidget* InHitNumber, FVector InLocation);

	UFUNCTION()
	void DestroyHitNumber(UUserWidget* InHitNumber);

	UFUNCTION()
	void UpdateHitNumbers();

	//UFUNCTION(BlueprintNativeEvent)
	void ShowHealthBar();
	//void ShowHealthBar_Implementation();

	//UFUNCTION(BlueprintNativeEvent)
	void HideHealthBar();
	//void HideHealthBar_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
	void ActivateDeadEffect();	

	UFUNCTION(BlueprintImplementableEvent)
	void ActivateDamageEffect();

	UFUNCTION()
	void SetCurrentEnemyStateType(EEnemyStateType InType);

public:
	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitNumber(int32 InDamage, FVector InHitLocation);

	static void SpawnEnemy(AActor* InSpawner, TSubclassOf<ACEnemy> InSpawnEnemyClass);
	
	void DestroyEnemy();

	void SpawnEnemyEffectWeapon();

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
	void MontageEnded();
	void OnStateTypeChange(EEnemyStateType InCurrentStateType);
	
	//FORCEINLINE bool GetbDamage() { return bDamage; }
	//FORCEINLINE void SetbMontageIsPlaying(bool InBool) { bMontageIsPlaying = InBool; }
	//FORCEINLINE bool GetbMontageIsPlaying() { return bMontageIsPlaying; }
	FORCEINLINE ACharacter* GetOpponent() { return Opponent; }
	//FORCEINLINE void SetCurrentEnemyStateType(EEnemyStateType InType) { CurrentStateType = InType; }
	FORCEINLINE EEnemyStateType GetCurrentEnemyStateType() { return CurrentStateType; }
	FORCEINLINE ACWeapon* GetWeapon(int32 InIndex) { if (Weapons[InIndex]) return Weapons[InIndex]; else return nullptr; }
	FORCEINLINE bool GetCanStrafing() { return CanStrafing; }
	//FORCEINLINE bool GetIsAttacking() { return IsAttacking; }
};