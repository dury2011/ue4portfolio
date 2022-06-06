/* MEMO: CDoAction_Boss.h 코드 배치 정리 완료 */
#pragma once

#include "CoreMinimal.h"
#include "Weapon/CDoAction.h"
#include "CDoAction_Boss.generated.h"

UCLASS()
class UE4POFOL_F_API UCDoAction_Boss : public UCDoAction
{
	GENERATED_BODY()
	
private:
	virtual void DoAction() override;

	virtual void OnWeaponBeginOverlap(class ACharacter* InAttacker, class AActor* InAttackCauser, class ACharacter* InOtherCharacter) override;

	virtual void OnWeaponEndOverlap(class ACharacter* InAttacker, class AActor* InAttackCauser, class ACharacter* InOtherCharacter) override;

	//virtual void OnWeaponHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};
