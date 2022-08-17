/* MEMO: CEnemy.h 코드 배치 정리 완료 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapon/CWeaponStructure.h"
#include "Component/CCharacterComponent.h"
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
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyDied);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyDiedStopAI);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyAttack);

UCLASS()
class UE4POFOL_F_API ACEnemy : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnEnemyDied OnEnemyDied;
	
	FOnEnemyStateTypeChanged OnEnemyStateTypeChanged;
	FOnEnemyAttackEnded OnEnemyAttackEnded;
	FOnEnemyMontageEnded OnEnemyMontageEnded;
	FOnEnemyMontageInterrupted OnEnemyMontageInterrupted;
	FOnEnemyParkourEnded OnEnemyParkourEnded;
	FOnEnemyDiedStopAI OnEnemyDiedStopAI;
	FOnEnemyAttack OnEnemyAttack;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	int32 ID = 0;

	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }

private:
	UPROPERTY()
	TArray<class UCapsuleComponent*> CapsuleCollisions;

	//UPROPERTY(VisibleDefaultsOnly)
	//class UBoxComponent* BoxComponentAttack;

	//UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	//class USphereComponent* SphereComponentDetectOpponent;

	UPROPERTY()
	class UBlackboardComponent* Blackboard;

	UPROPERTY()
	TArray<class ACWeapon*> Weapons;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Enemy Setting")
	float HealthBarDisplayTime = 1.5f;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Enemy Setting")
	float HitNumberDestroyTime = 1.0f;

	//UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Enemy Setting")
	//bool IsTest = false;

	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TMap<UUserWidget*, FVector> HitNumbers;
	
	UPROPERTY()
	class ACharacter* Opponent;

	UPROPERTY()
	class ACPlayer* Player;

	int32 SkillWeaponAttackCount = 0;
	static int32 SpawnCount;

	int32 TargettedActor = 0;
	
	float DistanceToOpponent;
	
	bool IsAttackByPlayer = false;
	bool IsAttackBySkillWeapon = false;
	bool IsLaunchBySkill = false;
	bool IsDeadBySkill = false;
	bool IsAttackBySpellFist = false;
	bool IsSkillStopHit = false;
	bool IsBoundUpBySkill = false;

	bool IsEnemyFriendAttackByEnemy = false;
	bool IsEnemyAttackByEnemyFriend = false;

	FVector CurrentLoc = FVector::ZeroVector;
	FVector StrafeDirection = FVector::ZeroVector;

	EEnemyStateType CurrentStateType = EEnemyStateType::Max;
	EEnemyStateType PreviousStateType = EEnemyStateType::Max;
	EEnemyStrafingType CurrentStrafingType = EEnemyStrafingType::Max;
	EEnemyStrafingType PreviousEnemyStrafingType = EEnemyStrafingType::Max;

	FTimerHandle HealthBarTimer;
	FTimerHandle StopTimer;
	FTimerHandle StrafeTimer;
	FTimerHandle BoundUpTimer;
	FTimerHandle SkillWeaponTimer;

	struct FDamaged
	{
		float DamageAmount;
		FActionDamageEvent* DamageEvent;
		class AController* EventInstigator;
		class AActor* DamageCauser;
	} Damaged;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TArray<FActionData> ActionDatas;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TArray<FDamageData> NormalDamageDatas;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TArray<FDamageData> SkillDamageDatas;
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Enemy Setting")
	TArray<FDamageData> BlockedDatas;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TArray<FActionData> DodgeDatas;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TArray<FDamageData> DeadDatas;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TArray<TSubclassOf<class ACWeapon>> WeaponClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TArray<TSubclassOf<class ACWeapon>> EffectWeaponClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TArray<TSubclassOf<class ACProjectile>> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TSubclassOf<class UMatineeCameraShake> DamageCameraShakeClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Enemy Setting")
	class UWidgetComponent* HealthBarWidgetComponent;

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

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Enemy Setting")
	bool InMission2 = false;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Friendly Setting")
	TArray<FActionData> FriendlyActionDatas;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	float StopTime = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	float ChangeStrafingTypeInterval = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	bool IsBoss = false;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Enemy Setting")
	bool ActivateDamageLaunch = true;
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Enemy Setting")
	bool IsPlayerFriendly = false;

	FGenericTeamId TeamId;

	bool bMontageIsPlaying = false;
	bool bActivateRotateToOpponent = true;
	bool CanStrafing = false;
	bool IsLaunching = false;
	bool IsOnce_HpZeroed = true;

public:
	ACEnemy();
	virtual void Destroyed() override;
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void OnAttack();
	void BeginStrafing();
	void ChangeStrafing();
	void EndStrafing();
	void BeginDodge();
	void OnStateTypeChange(EEnemyStateType InCurrentStateType);

	//////////
	void SetTarget(bool InBool, AActor* InActor);
	
	static ACEnemy* SpawnEnemy(AActor* InSpawner, TSubclassOf<ACEnemy> InSpawnEnemyClass);
	static ACEnemy* SpawnEnemy(AActor* InSpawner, TSubclassOf<ACEnemy> InSpawnEnemyClass, FName InSpawnSocketName);
	static ACEnemy* SpawnEnemy(AActor* InSpawner, TSubclassOf<ACEnemy> InSpawnEnemyClass, FTransform InSpawnTransform);

	UFUNCTION(BlueprintCallable)
	void DestroyEnemy();
	
	UFUNCTION()
	void TakeDamage_OpponentNormalAttack();
	
	UFUNCTION()
	void TakeDamage_OpponentUsingSkill();

	UFUNCTION()
	void TakeDamage_OpponentUsingSkillWeapon();

	UFUNCTION()
	void BlockedByShield();
	
	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitNumber(int32 InDamage, FVector InHitLocation);

	FORCEINLINE ACharacter* GetOpponent() { return Opponent; }
	FORCEINLINE void SetOpponent(bool InSuccess, ACharacter* InCharacter) { Opponent = InCharacter; TargettedActor++; }
	FORCEINLINE float GetDistanceToOpponent() { return DistanceToOpponent; }
	FORCEINLINE ACWeapon* GetWeapon(int32 InIndex) { if (Weapons[InIndex]) return Weapons[InIndex]; else return nullptr; }
	FORCEINLINE bool GetIsAttackByPlayer() { return IsAttackByPlayer; }
	FORCEINLINE EEnemyStateType GetCurrentEnemyStateType() { return CurrentStateType; }
	FORCEINLINE bool GetCanStrafing() { return CanStrafing; }
	FORCEINLINE void SetIsAttackByPlayer(bool InBool) { IsAttackByPlayer = InBool; }
	FORCEINLINE void SetIsAttackBySkillWeapon(bool InBool) { IsAttackBySkillWeapon = InBool; }
	FORCEINLINE void SetIsAttackBySpellFist(bool InBool) { IsAttackBySpellFist = InBool; }
	FORCEINLINE void SetSpawnCount() { ++SpawnCount; }
	FORCEINLINE int32 GetSpawnCount() { return SpawnCount; }
	FORCEINLINE void SetIsEnemyFriendAttackByEnemy(bool InBool) { IsEnemyFriendAttackByEnemy = InBool; }
	FORCEINLINE void SetIsEnemyAttackByEnemyFriend(bool InBool) { IsEnemyAttackByEnemyFriend = InBool; }
	FORCEINLINE void SetTargettedActorCount() { TargettedActor++; }
	FORCEINLINE int32 GetTargettedActorCount() { return TargettedActor; }

private:
	//void CheckDamage();
	void DamagedByOpponentNormal_SkillAndFx(float InLaunchSpeed = 1.0f);
	void DamagedByOpponentSkillWeaponAndFx();
	void GetNormalDamageData(int32 InIndex);
	void GetSkillDamageData(int32 InIndex);
	void CheckDead();

	UFUNCTION()
	void ChangeOpponentSpawn();

	void RecoverDilation();
	void ShakeCamera(FDamaged damage);

protected:
	virtual void BeginPlay() override;

	void ShowHealthBar();			
	void HideHealthBar();

	UFUNCTION(BlueprintCallable)
	void StoreHitNumber(UUserWidget* InHitNumber, FVector InLocation);
	
	UFUNCTION()
	void UpdateHitNumbers();
	
	UFUNCTION()
	void DestroyHitNumber(UUserWidget* InHitNumber);
	
	UFUNCTION()
	virtual void OnMontageEnded(UAnimMontage* InMontage, bool InInterrupted);
	
	UFUNCTION()
	void SetCurrentEnemyStateType(EEnemyStateType InType);

	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	virtual void OnBeginOverlapSphereComponentDetectOpponent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnEndOverlapSphereComponentDetectOpponent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintImplementableEvent)
	void ActivateDamageEffect(bool InGlow, EWeaponType InWeaponType);

	UFUNCTION(BlueprintImplementableEvent)
	void ActivateDamageIceEffect();
	
	UFUNCTION(BlueprintImplementableEvent)
	void DeactivateDamageIceEffect();
	
	UFUNCTION(BlueprintImplementableEvent)
	void ActivateDeadEffect();
};