#include "CAnimNotifyState_Collision.h"
#include "Global.h"
//#include "Component/CWeaponComponent.h"
#include "Weapon/CWeapon.h"
#include "Component/CBossComponent.h"
#include "Boss/CDataAsset_Boss.h"
#include "Enemy/CEnemy.h"
#include "Component/CCharacterComponent.h"
#include "Player/CPlayer.h"

FString UCAnimNotifyState_Collision::GetNotifyName_Implementation() const
{
	return "Collision";
}

void UCAnimNotifyState_Collision::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	
	CheckNull(MeshComp);
	CheckNull(MeshComp->GetOwner());

	UCBossComponent* bossComponent = CHelpers::GetComponent<UCBossComponent>(MeshComp->GetOwner());
	UCCharacterComponent* characterComponent = CHelpers::GetComponent<UCCharacterComponent>(MeshComp->GetOwner());

	if (bossComponent)
	{
		bossComponent->GetDataAsset()->GetWeapon()->OnCollision();

		return;
	}
	else if (characterComponent)
	{
		if(characterComponent->GetNormalWeapon(EWeaponType::Onehand))
			characterComponent->GetNormalWeapon(EWeaponType::Onehand)->OnCollision();
		
		if (characterComponent->GetNormalWeapon(EWeaponType::Spell))
			characterComponent->GetNormalWeapon(EWeaponType::Spell)->OnCollision();

		return;
	}
	else
	{
		ACEnemy* enemy = Cast<ACEnemy>(MeshComp->GetOwner());

		// MEMO: Enemy는 물리Weapon을 하나씩만 가지고 있음, Effect Weapon과 별개
		if (enemy)
			enemy->GetWeapon(0)->OnCollision();
	}
}

void UCAnimNotifyState_Collision::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	
	CheckNull(MeshComp);
	CheckNull(MeshComp->GetOwner());

	UCBossComponent* bossComponent = CHelpers::GetComponent<UCBossComponent>(MeshComp->GetOwner());
	UCCharacterComponent* characterComponent = CHelpers::GetComponent<UCCharacterComponent>(MeshComp->GetOwner());
	
	if (bossComponent)
	{
		bossComponent->GetDataAsset()->GetWeapon()->OffCollision();

		return;
	}
	else if (characterComponent)
	{
		if (characterComponent->GetNormalWeapon(EWeaponType::Onehand))
			characterComponent->GetNormalWeapon(EWeaponType::Onehand)->OffCollision();

		if (characterComponent->GetNormalWeapon(EWeaponType::Spell))
			characterComponent->GetNormalWeapon(EWeaponType::Spell)->OffCollision();

		return;
	}
	else
	{
		ACEnemy* enemy = Cast<ACEnemy>(MeshComp->GetOwner());

		if (enemy)
			enemy->GetWeapon(0)->OffCollision();
	}
}
