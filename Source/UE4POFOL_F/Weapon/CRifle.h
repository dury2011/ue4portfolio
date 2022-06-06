#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "CRifle.generated.h"

//TODO: Player의 ShowAmmoCount를 바인딩하고, Rifle이 발사 될때,델리게이트가 Broadcast되도록. 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRifleFire, int, FullAmmoCount, int, FireAmmouCount, int, LeftAmmoCount);

UCLASS()
class UE4POFOL_F_API ACRifle : public AActor
{
	GENERATED_BODY()

public:
	ACRifle();
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	static ACRifle* Spawn(class UWorld* InWorld, class ACharacter* InOwnerCharacter);

	UPROPERTY(BlueprintAssignable)
	FRifleFire OnRifleFire;

	UFUNCTION()
	void OnEquip();

	UFUNCTION()
	void OnUnequip();

	void Begin_Aim();
	void End_Aim();
	void Begin_Fire();
	void End_Fire();
	void ToggleAutoFire();
	void ChangeRifle();
	
	FORCEINLINE void SetAutoFire(bool Inbool) { bAutoFire = Inbool; }
	FORCEINLINE bool GetEquipped() { return bEquipped; } 
	FORCEINLINE bool GetAiming() { return bAiming; } 
	FORCEINLINE bool GetAutoFire() { return bAutoFire; } 

private:
	UPROPERTY(EditDefaultsOnly, Category = "Socket")
	FName HandSocket = "Rifle_Equipped";

	UPROPERTY(EditDefaultsOnly, Category = "Socket")
	FName HolsterSocket = "Rifle_Holster";

	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	class UCurveFloat* AimCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	float AimDistance = 3000;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	TSubclassOf<class ACProjectile> BulletClass;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	TSubclassOf<class UMatineeCameraShake> CameraShakeClass;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	float LimitPitchAngle = 0.25f; //45

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	class UParticleSystem* BulletParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	class USoundCue* FireSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	class UParticleSystem* FlashParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	class UMaterialInstanceConstant* ImpactPointDecal;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	class UParticleSystem* ImpactPointParticle;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	float AutoFireInterval = 0.1f;

	UPROPERTY(VisibleDefaultsOnly)
	class USkeletalMeshComponent* Mesh; // SkeletalMeshComponent 추가를 위한 Mesh.

	class ACharacter* OwnerCharacter;

	//class ACHUD* HUD;

	bool bEquipped;
	bool bAiming;
	bool bAutoFire;
	bool bEquipping;
	int FullAmmoCount;
	int FireAmmoCount;
	int LeftAmmoCount;
	bool bFiring;

	float PitchAngle;

	FTimerHandle FiringTimer;

	FTimeline Timeline;

	FOnTimelineFloat OnTimelineFloat;

	UFUNCTION()
	void Zooming(float Output);

	void Firing();

	//void BeginFireAI();
};
