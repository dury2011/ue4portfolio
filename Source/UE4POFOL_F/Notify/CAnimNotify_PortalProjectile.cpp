#include "Notify/CAnimNotify_PortalProjectile.h"
#include "Global.h"
#include "Player/CPlayer.h"

FString UCAnimNotify_PortalProjectile::GetNotifyName_Implementation() const
{
	return "Spawn Portal Projectile";
}

void UCAnimNotify_PortalProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());

	if (player)
		player->SpawnPortalProjectile();
}