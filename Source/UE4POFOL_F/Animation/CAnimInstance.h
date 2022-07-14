/* MEMO: CAniminstance.h 코드 배치 정리 완료 */
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
//#include "Component/CWeaponComponent.h"
//#include "Component/CAIStateComponent.h"
//#include "Component/CParkourComponent.h"
#include "Component/CIKComponent.h"
#include "Component/CCharacterComponent.h"
#include "CAnimInstance.generated.h"

UCLASS()
class UE4POFOL_F_API UCAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character Movement")
	float Speed;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	bool bInAir;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	float Pitch;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	float Roll;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	float Yaw;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	float YawDelta;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character Movement")
	float Direction;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character Movement")
	float ClimbUpSpeed;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character Movement")
	float ClimbRightSpeed;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	bool bRifleEquipped;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	bool bRifleAiming;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	bool bRifleEquippedAndHide;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	bool IsRunning;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	bool IsSpellTravel;

	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapon Type")
	//EWeaponType ActiveWeaponType = EWeaponType::Max;

	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Parkour Type")
	//EParkourType ActiveParkourType = EParkourType::Max;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Parkour")
	bool bActivateIK = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Animation")
	FFeetData FeetData;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Jump")
	bool bBeginJump = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "State")
	EStateType CurrentStateType;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "State")
	EStateType PreviousStateType;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "State")
	EWeaponType CurrentWeaponType;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "State")
	EWeaponType PreviousWeaponType;

	/////
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character Movement")
	bool bAccelerating = false;

	FRotator RotationLastTick;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character Movement")
	bool bFullBody = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character Movement")
	bool bTurnBack = false;

private:
	UPROPERTY()
	class ACharacter* OwnerCharacter;
	
	UPROPERTY()
	class UCCharacterComponent* CharacterComponent;

	UPROPERTY()
	class UCIKComponent* IKComponent;

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UFUNCTION()
	void OnStateTypeChanged(EStateType InCurrentType, EStateType InPreviousType);

	UFUNCTION()
	void OnWeaponTypeChanged(EWeaponType InCurrentType, EWeaponType InPreviousType);


	//UFUNCTION()
	//void OnWeaponTypeChanged(EWeaponType InPrevType, EWeaponType InNewType);

	//UFUNCTION()
	//void OnParkourTypeChanged(EParkourType InNewType);

protected:
/* ABP_Player의 이벤트 그래프에서 사용하려고 만들었다.
 * 이 함수를 통하여 Player가 Parkour하고 있다면,
 * ParkourType이 Hide이거나 Climb이면 true를 반환한다.
 * ParkourType이 위의 경우 이외이면 false를 반환한다.*/
	//UFUNCTION(BlueprintCallable, Category = "Parkour")
	//bool IsHideOrClimbMode();
};
