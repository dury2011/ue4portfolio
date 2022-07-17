#include "Notify/CAnimNotify_SpawnWeapon.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "Weapon/CWeapon.h"

FString UCAnimNotify_SpawnWeapon::GetNotifyName_Implementation() const
{
	return FString("Spawn Weapon");
}

void UCAnimNotify_SpawnWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);
	CheckNull(WeaponClass);

	ACharacter* character = Cast<ACharacter>(MeshComp->GetOwner());
	
	if (character)
	{
		ACWeapon* weapon = ACWeapon::SpawnWeapon(character, WeaponClass, character->GetMesh()->GetSocketLocation(SpawnSocketName));
		weapon->SetOwner(character);
	}
}