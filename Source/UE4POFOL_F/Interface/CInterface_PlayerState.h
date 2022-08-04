#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CInterface_PlayerState.generated.h"

UENUM(BlueprintType)
enum class EPlayerSkillType : uint8
{
	/*W1_S, W1_E, W2_S, W2_E, W3_S, W3_E, WC_S, WC_E, S1_S, S1_E, S2_S, S2_E, S3_S, S3_E, SC_S, SC_E, SF1_S, SF1_E, SF2_S, SF2_E, SF3_S, SF3_E,*/ NormalHit, BoundUpHit, AirborneHit, StopHit, FinalHit, Max
};

UENUM(BlueprintType)
enum class EPlayerNormalAttackType : uint8
{
	NormalAttack, BoundUpAttack, FinalAttack, Max
};

UENUM(BlueprintType)
enum class EPlayerSpellFistType : uint8
{
	SF_S, SF_E, Max
};

UINTERFACE(MinimalAPI)
class UCInterface_PlayerState : public UInterface
{
	GENERATED_BODY()
};

class UE4POFOL_F_API ICInterface_PlayerState
{
	GENERATED_BODY()

public:
	virtual float CurrentHp() { return 0.0f; };
	virtual float CurrentMp() { return 0.0f; };
	virtual float CurrentSp() { return 0.0f; };

	virtual float MaxHp() { return 0.0f; };
	virtual float MaxMp() { return 0.0f; };
	virtual float MaxSp() { return 0.0f; };

	virtual EPlayerNormalAttackType GetCurrentPlayerNormalAttackType() { return EPlayerNormalAttackType::Max; }
	virtual EPlayerSkillType GetCurrentPlayerSkillType() { return  EPlayerSkillType::Max; };
	virtual EPlayerSpellFistType GetCurrentPlayerSpellFistType() { return EPlayerSpellFistType::Max; };

	virtual bool GetPlayerActivateSkill() { return false; }
	virtual void SetPlayerActivateSkill(bool InBool) { };

	virtual bool GetPlayerIsAttackByBoss() { return false; }
	virtual void SetPlayerIsAttackByBoss(bool InBool) { };

	virtual void SetPlayerIsInBossStage(bool InBool) { };
	virtual bool GetPlayerUsingShield() { return false; }
};
