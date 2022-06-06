#include "Notify/CAnimNotify_End_DoAction.h"
#include "Global.h"
//#include "Component/CWeaponComponent.h"
#include "Weapon/CDoAction.h"

FString UCAnimNotify_End_DoAction::GetNotifyName_Implementation() const
{
	return "End DoAction";
}

void UCAnimNotify_End_DoAction::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	CheckNull(MeshComp);
	CheckNull(MeshComp->GetOwner());

	//UCWeaponComponent* weaponComponent = CHelpers::GetComponent<UCWeaponComponent>(MeshComp->GetOwner());
	//if (!!weaponComponent)
		//weaponComponent->GetDoAction()->End_DoAction();
}
