// MEMO: CAnimNotify_Unequip.cpp 코드 배치 정리 완료 
#include "Notify/CAnimNotify_Unequip.h"
#include "Global.h"
#include "Component/CCharacterComponent.h"
#include "Weapon/CWeapon.h"

FString UCAnimNotify_Unequip::GetNotifyName_Implementation() const
{
	return "Unequip";
}

void UCAnimNotify_Unequip::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	CheckNull(MeshComp);
	CheckNull(MeshComp->GetOwner());

	UCCharacterComponent* characterComponent = Cast<UCCharacterComponent>(MeshComp->GetOwner()->GetComponentByClass(UCCharacterComponent::StaticClass()));

	if (characterComponent)
	{
		if (characterComponent->GetIsWeaponOnehandMode())
			characterComponent->GetNormalWeapon(EWeaponType::Onehand)->AttachTo(characterComponent->GetUnEquipSocketName(EWeaponType::Onehand));
		else if (characterComponent->GetIsWeaponSpellMode())
			characterComponent->GetNormalWeapon(EWeaponType::Spell)->AttachTo(characterComponent->GetUnEquipSocketName(EWeaponType::Spell));
	}
}