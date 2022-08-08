#include "Notify/CAnimNotify_CannonProjectile.h"
#include "Global.h"
#include "Player/CCannon.h"

FString UCAnimNotify_CannonProjectile::GetNotifyName_Implementation() const
{
	return "Spawn Cannon Projectile";
}

void UCAnimNotify_CannonProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACCannon* cannon = Cast<ACCannon>(MeshComp->GetOwner());

	if (cannon)
		cannon->OnFireNormalProjectile();
}