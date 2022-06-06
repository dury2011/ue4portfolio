/* MEMO: CDoAction.cpp 코드 배치 정리 완료 */
#include "Weapon/CDoAction.h"
#include "GameFramework/Character.h"
#include "Global.h"

UCDoAction::UCDoAction()
{

}

void UCDoAction::BeginPlay(class ACharacter* InOwnerCharacter, class ACWeapon*InWeapon, TArray<FActionData>InActionDatas, TArray<FDamageData> InDamageDatas)
{
	OwnerCharacter = InOwnerCharacter;
	ActionDatas = InActionDatas;
	DamageDatas = InDamageDatas;
	World = OwnerCharacter->GetWorld();
	AnimInstance = CHelpers::GetComponent<USkeletalMeshComponent>(InOwnerCharacter)->GetAnimInstance();
	InOwnerCharacter->GetWorldTimerManager().SetTimer(TimerHandle, this, &UCDoAction::Timer, 0.1f, true);

	// StateComponent = CHelpers::GetComponent<UCStateComponent>(OwnerCharacter);
}
