/* MEMO: CAnimInstance_Enemy.h 코드 배치 정리 완료 
* 파일 네임 CAniminstance_Enemy로 변경해야 할 듯 */
#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enemy/CEnemy.h"
#include "CAnimInstance_Enemy.generated.h"

UCLASS()
class UE4POFOL_F_API UCAnimInstance_Enemy : public UAnimInstance
{
	GENERATED_BODY()
	
public:

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character Movement")
	float Speed;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character Movement")
	float Direction;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character Movement")
	float Pitch;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Chracter State")
	bool bFalling;

	UPROPERTY(BlueprintReadOnly)
	EEnemyStateType CurrentEnemyStateType;

	UPROPERTY(BlueprintReadOnly)
	EEnemyStateType PreviousEnemyStateType;

private:
	UPROPERTY()
	class ACEnemy* Enemy;
	
	//UPROPERTY()
	//TArray<class AActor*> ActorsTagEnemyBossFriend;
	
public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
private:
	void OnEnemyStateTypeChanged(EEnemyStateType InPreviousType, EEnemyStateType InCurrentType);

};
