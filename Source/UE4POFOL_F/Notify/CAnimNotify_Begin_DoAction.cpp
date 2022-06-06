#include "Notify/CAnimNotify_Begin_DoAction.h"
#include "CAnimNotify_Begin_DoAction.h"
#include "Global.h"
//#include "Component/CWeaponComponent.h"
#include "Weapon/CDoAction.h"

FString UCAnimNotify_Begin_DoAction::GetNotifyName_Implementation() const
{
	return "Begin DoAction";
}

void UCAnimNotify_Begin_DoAction::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	Super::Notify(MeshComp, Animation);
	CheckNull(MeshComp);
	CheckNull(MeshComp->GetOwner());

	//UCWeaponComponent* weaponComponent = CHelpers::GetComponent<UCWeaponComponent>(MeshComp->GetOwner());
	
	//if (!!weaponComponent)
		//weaponComponent->GetDoAction()->Begin_DoAction();
}
