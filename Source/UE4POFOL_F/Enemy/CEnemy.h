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
	Idle, Walk, Run, Parkour, Attack, Damage, Dead, Max
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyStateTypeChanged, EEnemyStateType, InPreviousType, EEnemyStateType, InCurrentType);
DECLARE_MULTICAST_DELEGATE(FOnEnemyMontageEnded);

UCLASS()
class UE4POFOL_F_API ACEnemy : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	//UPROPERTY(BlueprintAssignable)
	FOnEnemyStateTypeChanged OnEnemyStateTypeChanged;

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
	TArray<TSubclassOf<class ACWeapon>> WeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TArray<TSubclassOf<class ACProjectile>> ProjectileClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TSubclassOf<class UMatineeCameraShake> DamageCameraShakeClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	TSubclassOf<class UAnimInstance> AnimBlueprint;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy Setting")
	class UWidgetComponent* HealthBarWidget;

	//TODO: UPROPERTY(BlueprintAssignable)메크로 안 써도 되나?
	FOnEnemyMontageEnded OnEnemyMontageEnded;
	
	bool bDamage = false;
	bool bMontageIsPlaying = false;
	bool bActivateRotateToOpponent = true;

private:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Enemy Setting")
	float HealthBarDisplayTime;
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Enemy Setting")
	float HitNumberDestroyTime;
	
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TMap<UUserWidget*, FVector> HitNumbers;
	
	UPROPERTY()
	TArray<class UCapsuleComponent*> CapsuleCollisions;
	
	UPROPERTY()
	class UBlackboardComponent* Blackboard;
	
	UPROPERTY()
	class ACharacter* Opponent;
	
	EEnemyStateType CurrentStateType = EEnemyStateType::Max;

	FTimerHandle HealthBarTimer;

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

private:
	void Damage();
	void Dead();

public:
	//TODO: 파생 클래스에서 재정의하여, 공격 관련 함수 대체하기
	virtual void OnAttack();

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

	UFUNCTION(BlueprintNativeEvent)
	void HideHealthBar();
	void HideHealthBar_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
	void ActivateDissolve();	

public:
	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitNumber(int32 InDamage, FVector InHitLocation);

	static void SpawnEnemy(AActor* InSpawner, TSubclassOf<ACEnemy> InSpawnEnemyClass);
	
	void DestroyEnemy();

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
	FORCEINLINE void SetCurrentEnemyStateType(EEnemyStateType InType) { CurrentStateType = InType; }
	FORCEINLINE EEnemyStateType GetCurrentEnemyStateType() { return CurrentStateType; }
};