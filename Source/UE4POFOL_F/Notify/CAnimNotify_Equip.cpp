// MEMO: CAnimNotify_Equip.cpp 코드 배치 정리 완료
#include "Notify/CAnimNotify_Equip.h"
#include "Global.h"
#include "Component/CCharacterComponent.h"
#include "Weapon/CWeapon.h"

FString UCAnimNotify_Equip::GetNotifyName_Implementation() const
{
	return "Equip";
}

void UCAnimNotify_Equip::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	
	CheckNull(MeshComp);
	CheckNull(MeshComp->GetOwner());

	UCCharacterComponent* characterComponent = Cast<UCCharacterComponent>(MeshComp->GetOwner()->GetComponentByClass(UCCharacterComponent::StaticClass()));

	if (characterComponent)
	{
		if (characterComponent->GetIsWeaponOnehandMode())
			characterComponent->GetNormalWeapon(EWeaponType::Onehand)->AttachTo(characterComponent->GetEquipSocketName(EWeaponType::Onehand));
		else if (characterComponent->GetIsWeaponSpellMode())
			characterComponent->GetNormalWeapon(EWeaponType::Spell)->AttachTo(characterComponent->GetEquipSocketName(EWeaponType::Spell));
	}
}