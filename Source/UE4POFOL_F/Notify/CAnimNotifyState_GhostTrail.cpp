#include "Notify/CAnimNotifyState_GhostTrail.h"
#include "Global.h"
#include "Effect/CGhostTrail.h"

FString UCAnimNotifyState_GhostTrail::GetNotifyName_Implementation() const
{
	return FString("Ghost Trail");
}

void UCAnimNotifyState_GhostTrail::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	CheckNull(MeshComp);

	ACharacter* character = Cast<ACharacter>(MeshComp->GetOwner());

	if (character)
	{
		GhostTrail = ACGhostTrail::SpawnGhostTrail(character, GhostTrailClass);
		GhostTrail->SetOwner(Cast<AActor>(character));
		GhostTrail->ToggleOn();
	}
}

void UCAnimNotifyState_GhostTrail::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	CheckNull(MeshComp);

	ACharacter* character = Cast<ACharacter>(MeshComp->GetOwner());

	if (character)
		GhostTrail->DeleteTrail();
}