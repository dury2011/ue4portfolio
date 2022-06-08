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

UCLASS()
class UE4POFOL_F_API ACCannon : public ACharacter
{
	GENERATED_BODY()

private:
	//UPROPERTY(EditDefaultsOnly, Category = "Setting Cannon")
	//FActionData FireData;

	UPROPERTY()
	class ACTriggerVolume_Cannon* TriggerVolume;

	UPROPERTY()
	class ACPlayer* Player;

	UPROPERTY()
	class AController* PlayerController;
	
	ECannonStateType CurrentCannonType = ECannonStateType::Max;
	ECannonStateType PreviousCannonType = ECannonStateType::Max;

	float Yaw;
	float Pitch;
	float Roll;

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

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Setting Cannon")
	TSubclassOf<class ACProjectile> CannonProjectileClass;

	UPROPERTY(BlueprintReadOnly)
	class ACProjectile* CannonProjectile;

	UPROPERTY(BlueprintAssignable)
	FOnCannonStateTypeChange OnCannonStateTypeChange;
	
public:
	ACCannon();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SpawnCannonProjectile();

	UFUNCTION(BlueprintImplementableEvent)
	void Fire();

	UFUNCTION(BlueprintImplementableEvent)
	void ActivateCannonUtil();

	UFUNCTION(BlueprintImplementableEvent)
	void DeactivateCannonUtil();

private:
	UFUNCTION()
	void PossessCannon(class AActor* InOverlappedTriggerVolume, class AActor* InOtherActor);
	void UnpossessCannon();
	void OnFire();
	void OnVerticalLook(float AxisValue);
	void OnHorizontalLook(float AxisValue);

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	void CannonStateTypeChange(ECannonStateType InType);
};
