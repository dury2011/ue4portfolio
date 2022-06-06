#include "CAnimNotifyState_Collision.h"
#include "Global.h"
//#include "Component/CWeaponComponent.h"
#include "Weapon/CWeapon.h"
#include "Component/CBossComponent.h"
#include "Boss/CDataAsset_Boss.h"

FString UCAnimNotifyState_Collision::GetNotifyName_Implementation() const
{
	return "Collision";
}

void UCAnimNotifyState_Collision::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	
	CheckNull(MeshComp);
	CheckNull(MeshComp->GetOwner());

	//UCWeaponComponent* weaponComponent = CHelpers::GetComponent<UCWeaponComponent>(MeshComp->GetOwner());
	UCBossComponent* bossComponent = CHelpers::GetComponent<UCBossComponent>(MeshComp->GetOwner());

	//if (weaponComponent)
		//weaponComponent->GetWeapon()->OnCollision();
	if (bossComponent)
		bossComponent->GetDataAsset()->GetWeapon()->OnCollision();

}

void UCAnimNotifyState_Collision::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	
	CheckNull(MeshComp);
	CheckNull(MeshComp->GetOwner());

	//UCWeaponComponent* weaponComponent = CHelpers::GetComponent<UCWeaponComponent>(MeshComp->GetOwner());
	UCBossComponent* bossComponent = CHelpers::GetComponent<UCBossComponent>(MeshComp->GetOwner());

	//if (!!weaponComponent)
		//if(!!weaponComponent->GetWeapon())
			//weaponComponent->GetWeapon()->OffCollision();
	
	if (!!bossComponent)
		if(!!bossComponent->GetDataAsset()->GetWeapon())
			bossComponent->GetDataAsset()->GetWeapon()->OffCollision();
}
