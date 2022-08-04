/* MEMO: CWeapon.h 코드 배치 정리 완료 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FWeaponOverlap, class ACharacter*, InAttacker, class AActor*, InAttackCauser, class ACharacter*, InOtherCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWeaponCollision);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FWeaponHit, class UPrimitiveComponent*, HitComponent, class AActor*, OtherActor, class UPrimitiveComponent*, OtherComp, FVector, NormalImpulse, const FHitResult&, Hit);
//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSkillWeaponBeginOverlap);

UCLASS()
class UE4POFOL_F_API ACWeapon : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FWeaponOverlap OnWeaponBeginOverlap;

	UPROPERTY(BlueprintAssignable)
	FWeaponOverlap OnWeaponEndOverlap;

	//UPROPERTY(BlueprintAssignable)
	//FSkillWeaponBeginOverlap OnSkillWeaponBeginOverlap;

	//UPROPERTY(BlueprintAssignable)
	//FWeaponOverlap OnWeaponBeginOverlapForCritical;

	//UPROPERTY(BlueprintAssignable)
	//FWeaponOverlap OnWeaponEndOverlapForCritical;

	//UPROPERTY(BlueprintAssignable)
	//FWeaponOverlap OnWeaponBeginOverlapForSkill;

	//UPROPERTY(BlueprintAssignable)
	//FWeaponOverlap OnWeaponEndOverlapForSkill;

	UPROPERTY(BlueprintAssignable)
	FWeaponCollision OnWeaponCollision;

	UPROPERTY(BlueprintAssignable)
	FWeaponCollision OffWeaponCollision;

	//UPROPERTY(BlueprintAssignable)
	//FWeaponCollision OnWeaponCollisionForCritical;

	//UPROPERTY(BlueprintAssignable)
	//FWeaponCollision OffWeaponCollisionForCritical;

	//UPROPERTY(BlueprintAssignable)
	//FWeaponCollision OnWeaponCollisionForSkill;

	//UPROPERTY(BlueprintAssignable)
	//FWeaponCollision OffWeaponCollisionForSkill;

	UPROPERTY(BlueprintAssignable)
	FWeaponHit OnWeaponHit;

	//bool Spawned = false;

private:
	UPROPERTY(VisibleDefaultsOnly)
	class USceneComponent* Root;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Setting")
	int32 PureDamage = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Setting")
	float CriticalPercentage = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Setting")
	int32 CriticalDamage = 0.0f;

	/* Add 0~RandomDeviation To Apply Damage */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon Setting")
	int32 RandomDeviation = 0.0f;

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon Setting")
	int32 ApplyDamage = 0.0;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Setting")
	bool IsSkillWeapon = false;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Setting")
	bool IsShield = false;

	//class ACEnemy* Enemy;

	FTimerHandle ComboCountTimer;
	FTimerHandle StopTimer;
	bool IsOverlapped = false;

protected:
	UPROPERTY(BlueprintReadOnly)
	class ACharacter* OwnerCharacter;

	UPROPERTY(BlueprintReadOnly)
	TArray<class UShapeComponent*> Collisions;

	UPROPERTY(BlueprintReadOnly)
	class UCapsuleComponent* capsuleCollision;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon Setting")
	float StopTime = 0.2f;

public:	
	ACWeapon();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	static ACWeapon* SpawnWeapon(ACharacter* InSpawner, TSubclassOf<ACWeapon> InWeaponClass, FVector InSpawnLocation);

	UFUNCTION(BlueprintNativeEvent)
	void DestroyWeapon();
	void DestroyWeapon_Implementation();

	UFUNCTION(BlueprintCallable)
	void AttachTo(FName InSocketName); 
	
	UFUNCTION(BlueprintCallable)
	void AttachCollisionTo(FName InSocketName);

	UFUNCTION(BlueprintCallable)
	void OnCollision();

	UFUNCTION(BlueprintCallable)
	void OffCollision();

private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void RecoverDilation();

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnEquip();

	UFUNCTION(BlueprintImplementableEvent)
	void OnUnequip();

	//virtual void Destroyed() override;
};
