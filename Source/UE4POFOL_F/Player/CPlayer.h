/* MEMO: CPlayer.h 코드 배치 정리 완료
* public: 변수, private: 변수, 함수 순으로 코드를 작성하였다.
* 함수는 .cpp에 나열된 함수 정의 순서로 함수 원형을 작성하였다.
*/
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "Component/CParkourComponent.h"
#include "GenericTeamAgentInterface.h"
#include "Weapon/CWeaponStructure.h"
#include "Components/TimelineComponent.h"
#include "Interface/CInterface_PlayerState.h"
#include "CPlayer.generated.h"


UENUM(BlueprintType)
enum class ECameraEffectType : uint8
{
	Damage, LowHp, Teleport, Max
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerCollision);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerOverlap, class ACharacter*, InAttacker, class AActor*, InAttackCauser, class ACharacter*, InOtherCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerActiveBlock, bool, IsBlocked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerSkillAttack);

UCLASS()
class UE4POFOL_F_API ACPlayer : public ACharacter, public IGenericTeamAgentInterface, public ICInterface_PlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadonly, VisibleDefaultsOnly)
	class UCameraComponent* CameraComponent;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	class UCCharacterComponent* CharacterComponent;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	class UCWidgetComponent* WidgetComponent;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	class UCIKComponent* IKComponent;

	// for skill and critical
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	class UBoxComponent* BoxComponentSkill2;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	class UBoxComponent* BoxComponentSkill3;
	
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	class USphereComponent* SphereComponentCritical;

	UPROPERTY(BlueprintReadOnly)
	TArray<class UCapsuleComponent*> CapsuleCollisions; 

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Player Setting")
	float ComboCountExistTime = 2.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Setting")
	int32 ComboCount = 0;

	UPROPERTY(BlueprintReadOnly)
	bool bPortalTeleporting = false;

	UPROPERTY(BlueprintReadOnly)
	ECameraEffectType CurrentCameraEffectType;
	
	FOnPlayerActiveBlock OnPlayerActiveBlock;

	FOnPlayerSkillAttack OnPlayerSkillAttack;

private:
	struct FDamaged
	{
		float DamageAmount; 
		FActionDamageEvent* DamageEvent;
		class AController* EventInstigator;
		class AActor* DamageCauser;
	} Damaged;

	struct FSkilledEnemy
	{
		class ACEnemy* SkilledEnemy;
	
	}Skilled;

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
	
	UPROPERTY(EditDefaultsOnly, Category = "Player Setting")
	TSubclassOf<class ACProjectile> WarriorSkill1ProjectileClass;
	UPROPERTY()
	class ACProjectile* WarriorSkill1Projectile;
	
	UPROPERTY()
	TSubclassOf<class ACWeapon> SpellMeteorClass;
	UPROPERTY()
	class ACWeapon* SpellMeteorWeapon;

	UPROPERTY()
	TArray<AActor*> OutTargettingActorArr;

	//TODO: 컴포넌트에 넣으면 핫 리로드 이슈 생길 수도 있어서 일단 임시로 Player 클래스에 직접 선언함
	UPROPERTY(EditDefaultsOnly, Category = "Player Setting")
	TSubclassOf<class UMatineeCameraShake> DamageCameraShakeClass;

	EPlayerSkillType CurrentPlayerSkillType;
	FTimerHandle ComboCountTimer;
	FTimerHandle WarriorSkillTimer;
	int32 Index = 0;
	int32 IndexTargetting = 0;
	int32 JumpCount = 0;
	FVector TargetLocation;
	FRotator TargetRotator;
	float Zooming;
	float ZoomInterpSpeed = 2.0f;
	bool bAiming = false;
	bool bCanCritical = false;;
	bool bUnequipping = false;
	bool bChanging = false;
	bool bOnCriticalReady = false;
	bool bCanNextAction;
	bool bAttacking = false;
	bool bCanCombo = false;
	bool IsActivateSkill = false;
	bool IsSpellTravel = false;
	
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
	void OnJump();
	void OffJump();
	void OnAim();
	void OffAim();
	void OnRun();
	void OffRun();
	void OnSpellTravel();
	void OnParkour();
	void OnAction();
	void OnSkillOne();
	void OffSkillOne();
	void OnSkillTwo();
	void OnSkillThree();
	void OnCritical();

public:
	void OnSkillAttack();
	void SpawnWarriorSkillOneProjectile();
	void SpawnSpellMeteorWeapon();
	void SpawnGhostTrail();
	void SetPlayerPortalLocation();

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnCameraEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnTravelModeEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void DestroyTravelModeEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnWarriorSkillOneEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void PlaySpellPortalOnAimSound();

	UFUNCTION(BlueprintImplementableEvent)
	void PlaySpellPortalOffAimSound();

	void BeginNextAction();
	void EndThisAction();

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
	void OnBoxSkill2BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxSkill2EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void OnBoxSkill3BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxSkill3EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnSphereCriticalBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereCriticalEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void MontageEnded(UAnimMontage* InMontage, bool Ininterrupted);

public:
	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(TeamId); }

private:
	void OnControllerRotationYaw_Debug();

public:
	void PlayerLog_Debug();


private:
	FVector CalculateMeshSocketToVectorLocation(FName InSocketName, FVector InDirectionTo);
	void ShakeCamera();

public:
	// ICInterface_PlayerState에서 override
	virtual float CurrentHp() override;
	virtual float CurrentMp() override;
	virtual float CurrentSp() override;

	virtual float MaxHp() override;
	virtual float MaxMp() override;
	virtual float MaxSp() override;
	virtual EPlayerSkillType GetCurrentPlayerSkillType() override;
	void SetCurrentPlayerSkillType(EPlayerSkillType InType); 

	virtual bool GetPlayerActivateSkill() override;
	virtual void SetPlayerActivateSkill(bool InBool) override;
	
	FORCEINLINE bool GetbAiming() { return bAiming; }
	FORCEINLINE bool GetbAttacking() { return bAttacking; }
	FORCEINLINE void SetbCanCombo(bool Inbool) { bCanCombo = Inbool; }
	FORCEINLINE bool GetbCanCombo() { return bCanCombo; }
	FORCEINLINE void SetIncreaseIndex() { Index++; }
	FORCEINLINE bool GetIsSpellTravel() { return IsSpellTravel; }
	FORCEINLINE void SetIsSpellTravel(bool InBool) { IsSpellTravel = InBool; }
};