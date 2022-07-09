#include "Notify/CAnimNotify_SpawnPortalDoor.h"
#include "Global.h"
#include "Object/CPortalDoor.h"
#include "GameFramework/Character.h"

FString UCAnimNotify_SpawnPortalDoor::GetNotifyName_Implementation() const
{
	return FString("Spawn Portal Door");
}

void UCAnimNotify_SpawnPortalDoor::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);
	CheckNull(PortalDoorClass);

	ACharacter* character = Cast<ACharacter>(MeshComp->GetOwner());

	if (character)
		ACPortalDoor::SpawnPortalDoor(character, character->GetMesh()->GetSocketLocation(SpawnSocketName), PortalDoorClass);
}