/* MEMO: CWeapon.h 코드 배치 정리 완료 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CWeapon.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FWeaponOverlap, class ACharacter*, InAttacker, class AActor*, InAttackCauser, class ACharacter*, InOtherCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWeaponCollision);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FWeaponHit, class UPrimitiveComponent*, HitComponent, class AActor*, OtherActor, class UPrimitiveComponent*, OtherComp, FVector, NormalImpulse, const FHitResult&, Hit);

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

private:
	UPROPERTY(EditAnywhere)
	float Damage;

	UPROPERTY(VisibleDefaultsOnly)
	class USceneComponent* Root;

	FTimerHandle ComboCountTimer;

protected:
	UPROPERTY(BlueprintReadOnly)
	class ACharacter* OwnerCharacter;

	UPROPERTY(BlueprintReadOnly)
	TArray<class UShapeComponent*> Collisions;

	UPROPERTY(BlueprintReadOnly)
	class UCapsuleComponent* capsuleCollision;

public:	
	ACWeapon();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	static ACWeapon* SpawnWeapon(ACharacter* InSpawner, TSubclassOf<ACWeapon> InWeaponClass, FVector InSpawnLocation);

	UFUNCTION(BlueprintImplementableEvent)
	void DestroyWeapon();

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

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnEquip();

	UFUNCTION(BlueprintImplementableEvent)
	void OnUnequip();
};
