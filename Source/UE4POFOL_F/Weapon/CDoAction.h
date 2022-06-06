/* MEMO: CDoAction.h 코드 배치 정리 완료 */
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Weapon/CWeaponStructure.h"
#include "CDoAction.generated.h"

UCLASS(Blueprintable)
class UE4POFOL_F_API UCDoAction : public UObject
{
	GENERATED_BODY()
	
protected:
	//UPROPERTY(BlueprintReadOnly)
	//class UCStateComponent* StateComponent;

	UPROPERTY(BlueprintReadOnly)
	class UAnimInstance* AnimInstance;
	
	class ACharacter* OwnerCharacter;
	class UWorld* World;
	TArray<FActionData> ActionDatas;
	TArray<FDamageData> DamageDatas;
	FTimerHandle TimerHandle;
	const bool* bEquipping;

public:
	UCDoAction();

	void BeginPlay(class ACharacter* InOwnerCharacter, class ACWeapon* InWeapon, TArray<FActionData>InActionDatas, TArray<FDamageData> InDamageDatas);

	virtual void DoAction() { };
	virtual void Begin_DoAction() { };
	virtual void End_DoAction() { };

	virtual void Tick(float DeltaTime) { };
	
	UFUNCTION()
	virtual void OnWeaponBeginOverlap(class ACharacter* InAttacker, class AActor* InAttackCauser, class ACharacter* InOtherCharacter) {};

	UFUNCTION()
	virtual void OnWeaponEndOverlap(class ACharacter* InAttacker, class AActor* InAttackCauser, class ACharacter* InOtherCharacter) {};

	UFUNCTION()
	virtual void OnWeaponCollision() {};

	UFUNCTION()
	virtual void OffWeaponCollision() {}; 

	UFUNCTION()
	virtual void OnWeaponHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {}

	UFUNCTION()
	virtual void PauseAction() {};

	UFUNCTION()
	virtual void ResumeAction() {};
	
	UFUNCTION()
	virtual void Timer() {};
	
	UFUNCTION()
	virtual void ResetHitEnemy() {};

	FORCEINLINE void SetEquipping(const bool* InEquipping) { bEquipping = InEquipping; }
};
