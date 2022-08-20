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
#include "Interface/CInterface_Interaction.h"
#include "Enemy/CEnemy_Boss.h"
#include "CPlayer.generated.h"


UENUM(BlueprintType)
enum class ECameraEffectType : uint8
{
	Damage, LowHp, Teleport, Max
};

// 컴포넌트 핫 리로드 이슈가 있어서 여기에 만들었음
UENUM(BlueprintType)
enum class ESkillType : uint8
{
	Skill1, Skill2, Skill3, Max
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerCollision);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnPlayerOverlap, class ACharacter*, InAttacker, class AActor*, InAttackCauser, class ACharacter*, InOtherCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerActiveBlock, bool, IsBlocked);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerNormalAttack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerSkillAttack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerSpellFistAttack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerSkillLaunch);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkillCoolTimeCounting);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpawnPlayerFriend);

UCLASS()
class UE4POFOL_F_API ACPlayer : public ACharacter, public IGenericTeamAgentInterface, public ICInterface_PlayerState, public ICInterface_Interaction
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
	class UBoxComponent* BoxComponentSkill;

	//UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	//class UBoxComponent* BoxComponentSkill3;
	//
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	class USphereComponent* SphereComponentSpellFist;

	UPROPERTY(BlueprintReadOnly)
	TArray<class UCapsuleComponent*> CapsuleCollisions; 

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Player Setting")
	float ComboCountExistTime = 2.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Setting")
	int32 ComboCount = 0;

	UPROPERTY(BlueprintReadOnly)
	bool bParkouring = false;

	UPROPERTY(BlueprintReadOnly)
	bool bPortalTeleporting = false;

	UPROPERTY(BlueprintReadOnly)
	ECameraEffectType CurrentCameraEffectType;

	UPROPERTY(BlueprintAssignable)
	FOnSpawnPlayerFriend OnSpawnPlayerFriend;

	UPROPERTY(BlueprintAssignable)
	FOnSkillCoolTimeCounting OnSkillCoolTimeCounting;
	
	FOnPlayerActiveBlock OnPlayerActiveBlock;

	FOnPlayerNormalAttack OnPlayerNormalAttack;

	FOnPlayerSkillAttack OnPlayerSkillAttack;

	// MEMO: 추후 FOnPlayerNormalAttack으로 통합 예정 
	FOnPlayerSpellFistAttack OnPlayerSpellFistAttack;

	FOnPlayerSkillLaunch OnPlayerSkillLaunch;

	//FOnPlayerSkillWeaponAttack OnPlayerSkillWeaponAttack;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI")
	int32 ID = 1;

	UPROPERTY(BlueprintReadWrite)
	bool CanOnSkill1 = true;

	UPROPERTY(BlueprintReadWrite)
	bool CanOnSkill2 = true;

	UPROPERTY(BlueprintReadWrite)
	bool CanOnSkill3 = true;

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

	UPROPERTY()
	TArray<AActor*> SpellFistedActors;

	//TODO: 컴포넌트에 넣으면 핫 리로드 이슈 생길 수도 있어서 일단 임시로 Player 클래스에 직접 선언함
	UPROPERTY(EditDefaultsOnly, Category = "Player Setting")
	TSubclassOf<class UMatineeCameraShake> DamageCameraShakeClass;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EPlayerNormalAttackType CurrentPlayerNormalAttackType;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EPlayerSkillType CurrentPlayerSkillType;

	//UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EPlayerSpellFistType CurrentPlayerSpellFistType;

	FRotator ParkourTargetRot = FRotator::ZeroRotator;

	UPROPERTY()
	class ACEnemy_Boss* Boss;

	UPROPERTY()
	class ACEnemy* Enemy;

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
	bool IsMontagePlaying = false;

	bool IsPressedOnOnehand = false;
	bool IsPressedOnSpell = false;
	bool IsPressedOnSpellFist = false;

	bool IsAttackByBoss = false;
	bool IsAttackingBoss = false;

	bool CanOnAction = true;

	bool IsAttackByEnemy = false;

	bool IsDashing = false;
	
	UPROPERTY(EditDefaultsOnly, Category = "Player Setting")
	TSubclassOf<class ACProjectile> SpellThrowProjectileClass;

	UPROPERTY()
	class ACProjectile* SpellThrowProjectile;

	//UPROPERTY(EditDefaultsOnly, Category = "Player Setting")
	//TSubclassOf<class ACWeapon> Spell_FistSkill1WeaponClass;

	//UPROPERTY()
	//class ACWeapon* Spell_FistSkill1Weapon;

	//UPROPERTY(EditDefaultsOnly, Category = "Player Setting")
	//TSubclassOf<class ACWeapon> Spell_FistSkill2WeaponClass;

	//UPROPERTY()
	//class ACWeapon* Spell_FistSkill2Weapon;

protected:
	// MEMO: 컴포넌트가 가끔 핫 리로드 문제가 생겨서 일단 여기다가 만들었음
	UPROPERTY(EditDefaultsOnly, Category = "Player Setting")
	TArray<FDamageData> DamageDatas;

	UPROPERTY(EditDefaultsOnly, Category = "Player Spawn Setting")
	TArray<FActionData> SpawnDatas;

	UPROPERTY(EditDefaultsOnly, Category = "Player Setting")
	TArray<FActionData>WarriorNormalAttackBossDatas;

	UPROPERTY(EditDefaultsOnly, Category = "Player Setting")
	TArray<FActionData> SpellFistEquipData;

	UPROPERTY(EditDefaultsOnly, Category = "Player Setting")
	TArray<FActionData> SpellFistUnequipData;

	UPROPERTY(EditDefaultsOnly, Category = "Player Setting")
	TArray<FActionData> SpellFistDatas;

	UPROPERTY(EditDefaultsOnly, Category = "Player Setting")
	TArray<FActionData> SpellFistSkillDatas;

	UPROPERTY(EditDefaultsOnly, Category = "Player Setting")
	TArray<FActionData> ParkourDatas;

	UPROPERTY(EditDefaultsOnly, Category = "Player Setting")
	TSubclassOf<class ACWeapon> SpellFistWeaponClass;

	UPROPERTY(EditDefaultsOnly, Category = "Player Setting")
	TArray<FActionData> ShieldDatas;

	UPROPERTY(EditDefaultsOnly, Category = "Player Setting")
	TArray<FActionData> DashData;

	UPROPERTY()
	class ACWeapon* SpellFistWeapon;

public:
	ACPlayer();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)override;
	
	UFUNCTION()
	void TakeDamage_AttackByBoss(EBossAttackType InType);

	UFUNCTION()
	void TakeDamage_AttackByEnemy();
	
	void SetIsAttackByEnemy(bool InBool, ACEnemy* InEnemy);

	void EnableBind();

	void Notify_BeginNextAction();
	void Notify_EndThisAction();

	void Notify_OnNormalAttack();
	void Notify_OnSkillAttack();
	void Notify_OnSkillLaunch();
	void Notify_OnSpellFistAttack();

	void Notify_ParkourRotation();
	void Notify_OnParkourFinish();

	void Notify_SetCurrentPlayerNormalAttackType(EPlayerNormalAttackType InType);
	void Notify_SetCurrentPlayerSkillType(EPlayerSkillType InType);
	void Notify_SetCurrentPlayerSpellFistType(EPlayerSpellFistType InType);

	void SpawnWarriorSkillOneProjectile();
	void SpawnSpellMeteorWeapon();
	//void SpawnGhostTrail();
	
	void ShieldDefencing(ACEnemy* InAttacker);
	
	void SetPlayerPortalLocation();

	UFUNCTION()
	void MontageEnded(UAnimMontage* InMontage, bool Ininterrupted);

	UFUNCTION()
	void OnCollision();

	UFUNCTION()
	void OffCollision();

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnAttackBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnAttackBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnSphereSpellFistBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereSpellFistEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	// ICInterface_PlayerState에서 override
	virtual float CurrentHp() override;
	virtual float CurrentMp() override;
	virtual float CurrentSp() override;
	virtual float MaxHp() override;
	virtual float MaxMp() override;
	virtual float MaxSp() override;
	virtual EPlayerNormalAttackType GetCurrentPlayerNormalAttackType() override;
	virtual EPlayerSkillType GetCurrentPlayerSkillType() override;
	virtual EPlayerSpellFistType GetCurrentPlayerSpellFistType() override;
	virtual bool GetPlayerIsAttackByBoss() override;
	virtual void SetPlayerIsAttackByBoss(bool InBool) override;
	virtual void SetPlayerIsInBossStage(bool InBool) override;
	virtual bool GetPlayerUsingShield() override;
	virtual bool GetPlayerActivateSkill() override;
	virtual void SetPlayerActivateSkill(bool InBool) override;

	UFUNCTION(BlueprintImplementableEvent)
	void WeaponChanged();

	UFUNCTION(BlueprintImplementableEvent)
	void ActivateSkillCoolTime(EWeaponType OutCurrentWeaponType, ESkillType OutCurrentSkillType);

	//UFUNCTION(BlueprintImplementableEvent)
	//void ActivateCoolTimeSkillOne();

	//UFUNCTION(BlueprintImplementableEvent)
	//void ActivateCoolTimeSkillTwo();

	//UFUNCTION(BlueprintImplementableEvent)
	//void ActivateCoolTimeSkillThree();

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

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnSkillEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void DestroySkillEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void ActivateDamageEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void ActivateShieldEquipEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void ActivateShieldUnequipEffect();

	UFUNCTION(BlueprintImplementableEvent)
	void ActivateShieldDefenceEffect();

	FORCEINLINE bool GetbAiming() { return bAiming; }
	FORCEINLINE bool GetbAttacking() { return bAttacking; }
	FORCEINLINE void Notify_SetbCanCombo(bool Inbool) { bCanCombo = Inbool; }
	FORCEINLINE bool GetbCanCombo() { return bCanCombo; }
	FORCEINLINE void SetIncreaseIndex() { Index++; }
	FORCEINLINE bool GetIsSpellTravel() { return IsSpellTravel; }
	FORCEINLINE void SetIsSpellTravel(bool InBool) { IsSpellTravel = InBool; }
	FORCEINLINE bool GetIsParkouring() { return bParkouring; };
	FORCEINLINE ACWeapon* GetSpellFistWeapon() { return SpellFistWeapon; }
	virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(TeamId); }

private:
	void OnMoveForward(float AxisValue);
	void OnMoveRight(float AxisValue);
	void OnVerticalLook(float AxisValue);
	void OnHorizontalLook(float AxisValue);
	void OnZoom(float AxisValue);
	void InZooming(float Infloat);
	void OnAim();
	void OffAim();
	void OnRun();
	void OffRun();
	void OnJump();
	void OffJump();
	void OnDash();
	void OffDash();
	void OnParkour();
	void OffParkour();
	void OnOnehand();
	void OnSpell();
	void OnSpellTravel();
	void OnSpellFist();
	void OnSpawnFriend();
	void OnShield();
	void OffShield();
	void OnAction();
	void OnSkillOne();
	void OffSkillOne();
	void OnSkillTwo();
	void OnSkillThree();
	void OnCritical();
	void ShakeCamera();
	bool OnActionChecker();

	UFUNCTION(BlueprintCallable)
	void OnSkill1Checker(bool CanUseSkill);

	UFUNCTION(BlueprintCallable)
	void OnSkill2Checker(bool CanUseSkill);

	UFUNCTION(BlueprintCallable)
	void OnSkill3Checker(bool CanUseSkill);

	void GetDamageData(int32 InIndex);

	FVector CalculateMeshSocketToVectorLocation(FName InSocketName, FVector InDirectionTo);
	
	void Debug_PlayerLog();
	void Debug_OnControllerRotationYaw();

protected:
	virtual void BeginPlay() override;

};