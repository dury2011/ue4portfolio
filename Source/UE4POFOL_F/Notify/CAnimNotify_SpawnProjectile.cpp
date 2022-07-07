#include "Notify/CAnimNotify_SpawnProjectile.h"
#include "Global.h"
#include "Weapon/CProjectile.h"

FString UCAnimNotify_SpawnProjectile::GetNotifyName_Implementation() const
{
	return FString("Spawn Projectile");
}

void UCAnimNotify_SpawnProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);
	CheckNull(ProjectileClass);

	ACharacter* character = Cast<ACharacter>(MeshComp->GetOwner());

	if (character)
	{
		Projectile = ACProjectile::SpawnProjectile(character, ProjectileClass, SpawnSocketName);
		Projectile->SetOwner(MeshComp->GetOwner());
	}
}