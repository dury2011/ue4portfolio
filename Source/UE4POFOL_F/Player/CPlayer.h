/* MEMO: CPlayer.h 코드 배치 정리 완료
* public: 변수, private: 변수, 함수 순으로 코드를 작성하였다.
* 함수는 .cpp에 나열된 함수 정의 순서로 함수 원형을 작성하였다.
*/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/IRifle.h"
#include "Components/BoxComponent.h"
#include "Component/CParkourComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Weapon/CWeaponStructure.h"
#include "Components/TimelineComponent.h"
#include "CPlayer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerCollision);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerOverlap, class ACharacter*, InAttacker, class AActor*, InAttackCauser, class ACharacter*, InOtherCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerActiveBlock, bool, IsBlocked);

UCLASS()
class UE4POFOL_F_API ACPlayer : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadonly, VisibleDefaultsOnly)
	class UCameraComponent* CameraComponent;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	class UCCharacterComponent* CharacterComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	class UCWidgetComponent* WidgetComponent;
	
	//UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	//class UCParkourComponent* ParkourComponent;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	class UCIKComponent* IKComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Class Setting")
	TSubclassOf<class ACWeapon> OnehandSkillEffect1Class;

	UPROPERTY(EditDefaultsOnly, Category = "Class Setting")
	TSubclassOf<class ACWeapon> OnehandSkillEffect2Class;
	
	// Collision //////////////////////////////////////////////////
	UPROPERTY(BlueprintReadOnly)
	TArray<class UCapsuleComponent*> CollisionCapsules; // CapsuleCollisions로 수정
	
	UPROPERTY(BlueprintAssignable)
	FOnPlayerActiveBlock OnPlayerActiveBlock;
	
	// Other //////////////////////////////////////////////////
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "UI Setting")
	float ComboCountExistTime = 2.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI Setting")
	int32 ComboCount = 0;

	UPROPERTY(BlueprintReadOnly)
	bool bPortalTeleporting = false;

private:
	struct FDamaged
	{
		float DamageAmount;
		FActionDamageEvent* DamageEvent;
		class AController* EventInstigator;
		class AActor* DamageCauser;
	} Damaged;

	UPROPERTY()
	TSubclassOf<UAnimInstance> AnimInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Player Setting")
	class UCurveFloat* AimCurve;

	FTimeline Timeline;
	FOnTimelineFloat OnTimelineFloat;
	
	UPROPERTY()
	UAnimMontage * BlockAnimMontage;
	
	UPROPERTY(VisibleDefaultsOnly)
	class USceneComponent* ArrowGroup;
	
	UPROPERTY(VisibleDefaultsOnly)
	class UArrowComponent* Arrows[6];
	
	UPROPERTY(VisibleDefaultsOnly)
	class USpringArmComponent* SpringArmComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Player Setting")
	uint8 TeamId = 1;
	
	UPROPERTY(EditDefaultsOnly, Category = "Player Setting")
	FVector2D ZoomRange = FVector2D(120.0f, 2400.0f);
	
	UPROPERTY(EditDefaultsOnly, Category = "Player Setting")
	float ZoomSpeed = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Player Setting")
	float DashSpeed = 6000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Player Setting")
	float JumpDashEaseOutTime = 0.5f;

	UPROPERTY()
	TSubclassOf<class ACCrosshair> Crosshair_SpellMeteorClass;

	UPROPERTY()
	class ACCrosshair* Crosshair_SpellMeteor;

	UPROPERTY()
	TSubclassOf<class ACPortalCrosshair> PortalCrosshairClass;

	UPROPERTY()
	class ACPortalCrosshair* PortalCrosshair;

	UPROPERTY()
	TSubclassOf<class ACProjectile> PortalProjectileClass;
	
	UPROPERTY()
	TSubclassOf<class ACPortalDoor> PortalDoorEntranceClass;

	UPROPERTY()
	TSubclassOf<class ACPortalDoor> PortalDoorExitClass;

	UPROPERTY()
	TSubclassOf<class ACWeapon> SpellMeteorClass;

	UPROPERTY()
	class ACProjectile* PortalProjectile;

	UPROPERTY()
	class ACWeapon* SpellMeteorWeapon;
	
	UPROPERTY()
	class ACPortalDoor* PortalDoorEntrance;

	UPROPERTY()
	class ACPortalDoor* PortalDoorExit;

	UPROPERTY()
	class ACProjectile* SpellProjectileL;

	UPROPERTY()
	class ACProjectile* SpellProjectileR;
	
	FTimerHandle ComboCountTimer;
	float Zooming;
	float ZoomInterpSpeed = 2.0f;
	bool bAiming = false;
	bool bCanCritical = false;;
	bool bEquipping = false;
	bool bUnequipping = false;
	bool bChanging = false;
	bool bDashing = false;
	bool bJumping = false;
	bool bOnCriticalReady = false;

	UPROPERTY(EditDefaultsOnly, Category = "Player Setting")
	TSubclassOf<class ACProjectile> SpellThrowProjectileClass;

	UPROPERTY()
	class ACProjectile* SpellThrowProjectile;

public:
	ACPlayer();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void OnMoveForward(float AxisValue);
	void OnMoveRight(float AxisValue);
	void OnVerticalLook(float AxisValue);
	void OnHorizontalLook(float AxisValue);
	void OnZoom(float AxisValue);
	void InZooming(float Infloat);
	void OnAim();
	void OffAim();
	void OnJump();
	void OffJump();
	void OnRun();
	void OffRun();
	void OnDash();
	void OffDash();
	void OnParkour();
	void OnAction();
	void OnCriticalOne();
	void OffCriticalOne();
	void OnCriticalTwo();
	void OnSkill();

private:
	void ShootSpawnedProjectile(ACProjectile* InTargetProjectile, FName InShootSocketName);
	
public:
	void SpawnSpellProjectileL();
	void SpawnSpellProjectileR();
	void SpawnPortalProjectile();
	void SpawnSpellThrowProjectile();
	void SpawnSpellMeteorWeapon();

	void SpawnPortalDoorEntrance();
	void SpawnPortalDoorExit();
	
	void SetPlayerPortalLocation();

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnCameraEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void PlaySpellPortalOnAimSound();

	UFUNCTION(BlueprintImplementableEvent)
	void PlaySpellPortalOffAimSound();

	void BeginAction();
	void EndAction();

private:
	void OnOnehand();
	void OnSpell();
	void OnOnehandSpell();
	void OnShield();
	void ShieldDefencing();
	void OffShield();

public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void OnCollision();

	UFUNCTION()
	void OffCollision();
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void MontageEnded(UAnimMontage* InMontage, bool Ininterrupted);
	
	//UFUNCTION(BlueprintNativeEvent)
	//void ShowComboCount();
	//void ShowComboCount_Implementation();
	//
	//UFUNCTION(BlueprintNativeEvent)
	//void HideComboCount();
	//void HideComboCount_Implementation();
	//
	//UFUNCTION(BlueprintNativeEvent)
	//void OnCritical();
	//void OnCritical_Implementation();
	//
	//UFUNCTION(BlueprintNativeEvent)
	//void ShowCriticalImage();
	//void ShowCriticalImage_Implementation();
	
	void SpawnEmitter();

	void SpawnEmitter2();

public:
	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(TeamId); }

private:
	void OnControllerRotationYaw_Debug();

public:
	void PlayerLog_Debug();


private:
	FVector CalculateMeshSocketToVectorLocation(FName InSocketName, FVector InDirectionTo);

	FORCEINLINE bool GetbAiming() { return bAiming; }
};