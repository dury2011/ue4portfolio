/* MEMO: CDoAction_Combo.cpp 코드 배치 정리 완료 */
#include "Weapon/CDoAction_Combo.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
//#include "Component/CStateComponent.h"
#include "Animation/AnimInstance.h"
#include "Player/CPlayer.h"
#include "Enemy/CEnemy.h"

//#define LOG_UCDoAction_Combo

void UCDoAction_Combo::DoAction()
{
	Super::DoAction();

	CheckFalse(Datas.Num() > 0);

	if (bEnable == true)
	{
		bExist = true;
		bEnable = false;

		return;
	}

	//CheckTrue(StateComponent->IsAttackNormalMode());

	//StateComponent->SetAttackNormalMode();

	Datas[Index].PlayMontage(OwnerCharacter);
}

void UCDoAction_Combo::Begin_DoAction()
{
	Super::Begin_DoAction();

	CheckFalse(bExist);
	
	bExist = false;
	
	Datas[++Index].PlayMontage(OwnerCharacter);
}

void UCDoAction_Combo::End_DoAction()
{
	Super::End_DoAction();
	
	Datas[Index].EndAction(OwnerCharacter);

	Index = 0;
}

void UCDoAction_Combo::OnWeaponBeginOverlap(class ACharacter* InAttacker, class AActor* InAttackCauser, class ACharacter* InOtherCharacter)
{
	CheckNull(InOtherCharacter);
	
	Super::OnWeaponBeginOverlap(InAttacker, InAttackCauser, InOtherCharacter);
	
	//UCStateComponent* stateComponent = CHelpers::GetComponent<UCStateComponent>(OwnerCharacter);

	//CheckNull(stateComponent);

	//if (stateComponent->IsAttackNormalMode())
	//{
		//HitDatas[Index].SendDamage(OwnerCharacter, InAttackCauser, InOtherCharacter);  
		//HitDatas[Index].PlayHitStop(World);
		
		//Cast<ACPlayer>(InAttacker)->ShowComboCount(); 
	//Cast<ACPlayer>(InAttacker)->ShowCriticalImage_Implementation();
		
		//StateComponent->AddSpecial(100);

		//GLog->Log("UCDoAction_Combo->OnAttachmentBeginOverlap()");
	//}

#ifdef LOG_UCDoAction_Combo
	CLog::Print("UCDoAction_Combo::InAttacker : " + InAttacker->GetName(), 11);
	CLog::Print("UCDoAction_Combo::InAttackCauser : " + InAttackCauser->GetName(), 12);
	CLog::Print("UCDoAction_Combo::InOtherCharacter : " + InOtherCharacter->GetName(), 13);
	CLog::Print("UCDoAction_Combo::HitDatas.Montage: " + HitDatas[Index].Montage->GetName(), 14);
	CLog::Print("UCDoAction_Combo::Effect: " + HitDatas[Index].Effect->GetName(), 15);
#endif LOG_UCDoAction_Combo
}

void UCDoAction_Combo::OnWeaponEndOverlap(ACharacter* InAttacker, AActor* InAttackCauser, ACharacter* InOtherCharacter)
{
	CheckNull(InOtherCharacter);

	//UCStateComponent* stateComponent = CHelpers::GetComponent<UCStateComponent>(OwnerCharacter);

	//CheckNull(stateComponent);

	//if (stateComponent->IsAttackNormalMode())
	//{
		//Super::OnWeaponEndOverlap(InAttacker, InAttackCauser, InOtherCharacter);

		//GLog->Log("UCDoAction_Combo->OnAttachmentEndOverlap()");
	//}

#ifdef LOG_UCDoAction_Combo
	CLog::Print("End : " + InOtherCharacter->GetName(), 12);
#endif //LOG_UCDoAction_Combo
}

