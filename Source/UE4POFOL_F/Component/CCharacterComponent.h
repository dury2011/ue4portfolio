// MEMO: CCharacterComponent.h 코드 배치 정리 완료
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon/CWeaponStructure.h"
#include "CCharacterComponent.generated.h"

// MEMO: EEnemyStateType 우선순위: Idle == Move < Attack < Damage < Dead
UENUM(BlueprintType)
enum class EStateType : uint8
{
	Idle, Move, Attack, Damage, Dead, Max
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Unarmed, Onehand, Spell, Max
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStateTypeChange, EStateType, CurrentType, EStateType, PreviousType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponTypeChange, EWeaponType, CurrentType, EWeaponType, PreviousType);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE4POFOL_F_API UCCharacterComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FStateTypeChange OnStateTypeChange;

	UPROPERTY(BlueprintAssignable)
	FWeaponTypeChange OnWeaponTypeChange;

private:
	UPROPERTY()
	class ACharacter* OwnerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "Character Setting")
	float MaxHp = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Character Setting")
	float MaxMp = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Character Setting")
	float MaxSp = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Character Setting")
	float Hp = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Character Setting")
	float Mp = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Character Setting")
	float Sp = 0.0f;

	EStateType CurrentStateType = EStateType::Max;
	EStateType PreviousStateType = EStateType::Max;

	EWeaponType CurrentWeaponType = EWeaponType::Max;
	EWeaponType PreviousWeaponType = EWeaponType::Max;

	UPROPERTY(EditDefaultsOnly, Category = "Character Setting")
	FName EquipSocketName[(int32)EWeaponType::Max];

	UPROPERTY(EditDefaultsOnly, Category = "Character Setting")
	FName UnEquipSocketName[(int32)EWeaponType::Max];

	UPROPERTY(EditDefaultsOnly, Category = "Character Setting")
	FActionData EquipDatas[(int32)EWeaponType::Max];

	UPROPERTY(EditDefaultsOnly, Category = "Character Setting")
	FActionData UnequipDatas[(int32)EWeaponType::Max];

	UPROPERTY(EditDefaultsOnly, Category = "Character Setting")
	TArray<FActionData> ActionDatasOnehand;

	UPROPERTY(EditDefaultsOnly, Category = "Character Setting")
	TArray<FActionData> ActionDatasSpell;

	UPROPERTY(EditDefaultsOnly, Category = "Character Setting")
	TArray<FActionData> CriticalDatasOnehand;

	UPROPERTY(EditDefaultsOnly, Category = "Character Setting")
	TArray<FActionData> CriticalDatasSpell;

	UPROPERTY(EditDefaultsOnly, Category = "Character Setting")
	TArray<FActionData> SkillDatasOnehand;

	UPROPERTY(EditDefaultsOnly, Category = "Character Setting")
	TArray<FActionData> SkillDatasSpell;

	UPROPERTY(EditDefaultsOnly, Category = "Character Setting")
	TArray<FDamageData> DamageDatas;

	UPROPERTY(EditDefaultsOnly, Category = "Character Setting")
	TSubclassOf<class ACWeapon> NormalWeaponClasses[(int32)EWeaponType::Max];

	UPROPERTY(EditDefaultsOnly, Category = "Character Setting")
	TSubclassOf<class ACWeapon> SkillWeaponClasses[(int32)EWeaponType::Max];

	UPROPERTY(EditDefaultsOnly, Category = "Character Setting")
	TSubclassOf<class ACWeapon> SeconderyWeaponclasses[(int32)EWeaponType::Max];

	UPROPERTY(EditDefaultsOnly, Category = "Character Setting")
	TSubclassOf<class ACProjectile> ProjectileClass;	
	
	UPROPERTY(EditDefaultsOnly, Category = "Character Setting")
	TSubclassOf<class ACGhostTrail> GhostTrailClass;

	UPROPERTY()
	ACWeapon* NormalWeapon[(int32)EWeaponType::Max];

	UPROPERTY()
	ACWeapon* SkillWeapon[(int32)EWeaponType::Max];

	UPROPERTY()
	ACWeapon* SeconderyWeapon;

	UPROPERTY()
	class ACProjectile* Projectile;
	
	UPROPERTY()
	class ACGhostTrail* GhostTrail;

	bool IsMontagePlaying = false;
	bool bCanMove = true;
	bool bFixedCamera = true;

public:
	UCCharacterComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetHp(float InHp);
	void SetMp(float InMp);
	void SetSp(float InSp);

	// EEnemyStateType: Idle, Move, Attack, Damage, Dead, Max
	void SetCurrnetStateType(EStateType InType);
	// EEnemyWeaponType: Onehand, Rifle, Spell, Max
	void SetCurrentWeaponType(EWeaponType InType);

private:
	/* 특정 AnimInstance 클래스의 멤버 함수가 이 클래스의 F~TypeChange형 Delegate를 이용하여 바인드되었다면,
	 * 이 함수 몸체에서 Broadcast 함
	 * AnimInstance의 값을 바꿔주기 위한 함수임*/
	void ChangeStateType(EStateType InType);

	void ChangeWeaponType(EWeaponType InType);

public:
	FORCEINLINE FName GetEquipSocketName(EWeaponType InType) { return EquipSocketName[(int32) InType]; }
	FORCEINLINE FName GetUnEquipSocketName(EWeaponType InType) { return UnEquipSocketName[(int32)InType]; }
	
	FORCEINLINE FActionData GetEquipData(EWeaponType InType) { return EquipDatas[(int32)InType]; }
	FORCEINLINE FActionData GetUnequipData(EWeaponType InType) { return UnequipDatas[(int32)InType]; }
	FORCEINLINE FActionData GetActionDatasOnehand(int32 InIndex) { return ActionDatasOnehand[InIndex]; }
	FORCEINLINE FActionData GetActionDatasSpell(int32 InIndex) { return ActionDatasSpell[InIndex]; }
	FORCEINLINE FActionData GetCriticalDatasOnehand(int32 InIndex) { return CriticalDatasOnehand[InIndex]; }
	FORCEINLINE FActionData GetCriticalDatasSpell(int32 InIndex) { return CriticalDatasSpell[InIndex]; }
	FORCEINLINE FActionData GetSkillDatasOnehand(int32 InIndex) { return SkillDatasOnehand[InIndex]; }
	FORCEINLINE FActionData GetSkillDatasSpell(int32 InIndex) { return SkillDatasSpell[InIndex]; }
	FORCEINLINE FDamageData GetDamageData(int32 InIndex) { return DamageDatas[InIndex]; }

	FORCEINLINE TSubclassOf<class ACProjectile> GetProjectileClass() { return ProjectileClass; }

	FORCEINLINE ACWeapon* GetNormalWeapon(EWeaponType InType) { if (NormalWeapon[(int32)InType]) return NormalWeapon[(int32)InType]; else return nullptr; }
	FORCEINLINE ACWeapon* GetSkillWeapon(EWeaponType InType) { if (SkillWeapon[(int32)InType]) return SkillWeapon[(int32)InType]; else return nullptr; }
	FORCEINLINE ACWeapon* GetSeconderyWeapon() { if (SeconderyWeapon) return SeconderyWeapon; else return nullptr; }
	FORCEINLINE ACProjectile* GetProjectile() { if (Projectile) return Projectile; else return nullptr; }
	FORCEINLINE ACGhostTrail* GetGhostTrail() { if (GhostTrail) return GhostTrail; else return nullptr; }

	FORCEINLINE bool GetIsMontagePlaying() { return IsMontagePlaying; }
	FORCEINLINE void SetIsMontagePlaying(bool InIsMontagePlaying) { IsMontagePlaying = InIsMontagePlaying; }

	FORCEINLINE EStateType GetCurrnetStateType() { return CurrentStateType; }
	FORCEINLINE EWeaponType GetCurrentWeaponType() { return CurrentWeaponType; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetMaxHp() { return MaxHp; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetMaxMp() { return MaxMp; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetMaxSp() { return MaxSp; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentHp() { if (Hp <= 0.0f) return Hp = 0.0f; else return Hp; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentMp() { if (Mp <= 0.0f) return Mp = 0.0f; else return Mp; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentSp() { if (Sp <= 0.0f) return Sp = 0.0f; else return Sp; }
	
	FORCEINLINE bool GetIsStateIdleMode() { return CurrentStateType == EStateType::Idle; }
	FORCEINLINE bool GetIsStateMoveMode() { return CurrentStateType == EStateType::Move; }
	FORCEINLINE bool GetIsStateAttackMode() { return CurrentStateType == EStateType::Attack; }
	FORCEINLINE bool GetIsStateDamageMode() { return CurrentStateType == EStateType::Damage; }
	FORCEINLINE bool GetIsStateDeadMode() { return CurrentStateType == EStateType::Dead; }

	FORCEINLINE bool GetIsWeaponUnarmedMode() { return CurrentWeaponType == EWeaponType::Unarmed; }
	FORCEINLINE bool GetIsWeaponOnehandMode() { return CurrentWeaponType == EWeaponType::Onehand; }
	FORCEINLINE bool GetIsWeaponSpellMode() { return CurrentWeaponType == EWeaponType::Spell; }

	FORCEINLINE bool GetbCanMove() { return bCanMove; }
	FORCEINLINE void SetbCanMove(bool Inbool) { bCanMove = Inbool; }

	FORCEINLINE bool GetbFixedCamera() { return bFixedCamera; }
	FORCEINLINE void SetbFixedCamera(bool Inbool) { bFixedCamera = Inbool; }

};
