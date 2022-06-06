/* MEMO: CDataAsset_Boss.cpp 코드 배치 정리 완료 */
#include "Boss/CDataAsset_Boss.h"
#include "GameFramework/Character.h"
#include "Weapon/CWeapon.h"
#include "Boss/CBoss.h"
#include "Weapon/CDoAction.h"
#include "Global.h"

UCDataAsset_Boss::UCDataAsset_Boss()
{

}

void UCDataAsset_Boss::BeginPlay(class ACharacter* InOwnerCharacter)
{
	FActorSpawnParameters actorSpawnParams;

	actorSpawnParams.Owner = InOwnerCharacter;

	Boss = Cast<ACBoss>(actorSpawnParams.Owner);

	if (WeaponClass)
	{
		Weapon = InOwnerCharacter->GetWorld()->SpawnActor<ACWeapon>(WeaponClass, actorSpawnParams);
	}

	if (DoActionClass)
	{
		DoAction = NewObject<UCDoAction>(this, DoActionClass);
		DoAction->BeginPlay(InOwnerCharacter, Weapon, AttackMontagesAndDatas, DamageMontagesAndDatas);

		if (Weapon)
		{
			Weapon->OnEquip();

			Weapon->OnWeaponBeginOverlap.AddDynamic(DoAction, &UCDoAction::OnWeaponBeginOverlap);
			Weapon->OnWeaponBeginOverlap.AddDynamic(Boss, &ACBoss::OwnedWeaponBeginOverlap);
			Weapon->OnWeaponEndOverlap.AddDynamic(DoAction, &UCDoAction::OnWeaponEndOverlap);
			Weapon->OnWeaponEndOverlap.AddDynamic(Boss, &ACBoss::OwnedWeaponEndOverlap);
			Weapon->OnWeaponCollision.AddDynamic(DoAction, &UCDoAction::OnWeaponCollision);
			Weapon->OffWeaponCollision.AddDynamic(DoAction, &UCDoAction::OffWeaponCollision);
		}
	}
}