#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapon/CWeaponStructure.h"
#include "CCannon.generated.h"

UENUM(BlueprintType)
enum class ECannonStateType : uint8
{
	Unpossessed, Possessed, Fire, Damage, Dead, Max
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCannonStateTypeChange, ECannonStateType, InPreviousType, ECannonStateType, InCurrentType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCannonFire, int32, InCurrentAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCannonReloading, bool, InNowReloading);

UCLASS()
class UE4POFOL_F_API ACCannon : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	class UCWidgetComponent* WidgetComponent;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	class USkeletalMesh* SkeletalMesh;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	class UCameraComponent* Camera;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	class USpringArmComponent* SpringArm;

	UPROPERTY(BlueprintReadOnly)
	TArray<class UCapsuleComponent*> CapsuleCollisions;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Cannon Setting")
	float MaxHp = 10000.0;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Cannon Setting")
	float CurrentHp = 10000.0;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Cannon Setting")
	TSubclassOf<class ACProjectile> CannonProjectileClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Cannon Setting")
	TSubclassOf<class ACProjectile> CannonProjectileRangedClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Cannon Setting")
	TSubclassOf<class ACProjectile> HomingProjectileClass;

	UPROPERTY(BlueprintReadOnly)
	class ACProjectile* CannonProjectile;

	UPROPERTY(BlueprintAssignable)
	FOnCannonStateTypeChange OnCannonStateTypeChange;

	UPROPERTY(BlueprintAssignable)
	FOnCannonFire OnCannonFire;

	UPROPERTY(BlueprintAssignable)
	FOnCannonReloading OnCannonReloading;

	UPROPERTY(EditDefaultsOnly, Category = "Cannon Setting")
	float CriticalFireInterval = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Cannon Setting")
	float CriticalHomingProjFireInterval = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Cannon Setting")
	float RangedAvailTime = 4.0f;

private:
	//struct PlayerInfo
	//{
	//	float MaxHp;
	//	float CurrentHp;
	//	float MaxMp;
	//	float CurrentMp;
	//} PlayerInformation;
	//UPROPERTY(EditDefaultsOnly, Category = "Setting Cannon")
	//FActionData FireData;


	UPROPERTY()
	class ACTriggerVolume_Cannon* TriggerVolume;

	UPROPERTY()
	class ACPlayer* Player;

	UPROPERTY()
	class AController* PlayerController;

	UPROPERTY()
	ACProjectile* RangedProjectile;
	
	ECannonStateType CurrentCannonType = ECannonStateType::Max;
	ECannonStateType PreviousCannonType = ECannonStateType::Max;

	float Yaw;
	float Pitch;
	float Roll;

	int32 RangedAvailCounter = 0;
	int32 CriticalAvailCounter = 0;
	int32 ReloadCounter = 0;
	FTimerHandle FireTimer;
	FTimerHandle HomingTimer;
	FTimerHandle RangedTimer;
	FTimerHandle CriticalFinishTimer;
	FTimerHandle ReloadTimer;
	bool CanOnCriticalFire = false;
	bool ClickedOnFire = false;
	bool IsNowReloading = false;


protected:
	UPROPERTY(BlueprintReadOnly, Category = "Cannon Setting")
	int32 CurrentAmmo = 50;

public:
	ACCannon();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void OnFireNormalProjectile();
	
	UFUNCTION(BlueprintImplementableEvent)
	void ActivateNormalFireEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void ActivateCriticalFireEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void ActivatePossessCannonEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void DeactivatePossessCannonEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void ActivateZeroAmmoSound();

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentAmmo() {return CurrentAmmo;}

	UFUNCTION(BlueprintCallable)
	float GetHpPercentage();

private:
	void OnVerticalLook(float AxisValue);
	void OnHorizontalLook(float AxisValue);
	
	void OnFire();
	void OffFire();
	void OnReload();
	
	void FunctionBindForTimer_OnFireNormal();
	void SetCanCriticalFireTrue();
	void OnFireHomingProjectile();
	void SpawnRangedProjectile();
	void ShootRangedProjectile();
	void CannonStateTypeChange(ECannonStateType InType);
	void RangedAvailChecker();
	void CriticalAvailChecker();
	void ReloadChecker();
	
	UFUNCTION()
	void PossessCannon(class AActor* InOverlappedTriggerVolume, class AActor* InOtherActor);
	
	void UnpossessCannon();
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)override;



protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void ActivateCannonRangedAttackReadyEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void ActivateCannonRangedAttackFireEffect();

};
