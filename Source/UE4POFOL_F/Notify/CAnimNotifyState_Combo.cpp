#include "Notify/CAnimNotifyState_Combo.h"
#include "Global.h"
//#include "Component/CWeaponComponent.h"
//#include "Weapon/CDoAction_Combo.h"
#include "Player/CPlayer.h"

FString UCAnimNotifyState_Combo::GetNotifyName_Implementation() const
{
	return "Combo";
}

void UCAnimNotifyState_Combo::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	
	CheckNull(MeshComp);
	CheckNull(MeshComp->GetOwner());

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->SetbCanCombo(true);


	//UCWeaponComponent* weaponComponent = CHelpers::GetComponent<UCWeaponComponent>(MeshComp->GetOwner());
	//if (!!weaponComponent)
	//{
		//UCDoAction_Combo* combo = Cast<UCDoAction_Combo>(weaponComponent->GetDoAction());
	//
		//if (!!combo)
			//combo->Enable_Combo();
	//}
}

void UCAnimNotifyState_Combo::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	
	CheckNull(MeshComp);
	CheckNull(MeshComp->GetOwner());

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->SetbCanCombo(false);

	//UCWeaponComponent* weaponComponent = CHelpers::GetComponent<UCWeaponComponent>(MeshComp->GetOwner());
	//if (!!weaponComponent)
	//{
		//UCDoAction_Combo* combo = Cast<UCDoAction_Combo>(weaponComponent->GetDoAction());
	//
		//if (!!combo)
			//combo->Disable_Combo();
	//}
}


