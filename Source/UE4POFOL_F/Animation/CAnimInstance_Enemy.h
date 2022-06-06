/* MEMO: CAnimInstance_Enemy.h 코드 배치 정리 완료 
* 파일 네임 CAniminstance_Enemy로 변경해야 할 듯 */
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CAnimInstance_Enemy.generated.h"

UCLASS()
class UE4POFOL_F_API UCAnimInstance_Enemy : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	//UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	//EAIStateType AIStateType;

	//UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	//EAIStateType_Onehand AIStateTypeOnehand;

	//UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	//EAIStateType_Rifle AIStateTypeRifle;

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character Movement")
	float Speed;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character Movement")
	float Direction;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character Movement")
	float Pitch;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Chracter State")
	bool bFalling;

	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Chracter State")
	//bool bRifleEquipped;

	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Chracter State")
	//bool bRifleAiming;

	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Chracter State")
	//bool bRifleEquippedAndHide;

private:
	UPROPERTY()
	TArray<class AActor*> ActorsTagEnemyBossFriend;
	
	UPROPERTY()
	class ACharacter* OwnerCharacter;
	
	//UPROPERTY()
	//class UCAIStateComponent* AIStateComponent;

	//UPROPERTY()
	//class UCWeaponComponent* WeaponComponent;

	//UPROPERTY()
	//class UCParkourComponent* ParkourComponent;

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
private:
	//UFUNCTION()
	//void OnAIStateTypeChanged(EAIStateType InNewType);

	//UFUNCTION()
	//void OnAIStateTypeOnehandChanged(EAIStateType_Onehand InNewType);

	//UFUNCTION()
	//void OnAIStateTypeRifleChanged(EAIStateType_Rifle InNewType);
};
