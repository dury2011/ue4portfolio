#include "Notify/CAnimNotifyState_GhostTrail.h"
#include "Global.h"
#include "Effect/CGhostTrail.h"
#include "GameFramework/Character.h"

FString UCAnimNotifyState_GhostTrail::GetNotifyName_Implementation() const
{
	return FString("Ghost Trail");
}

void UCAnimNotifyState_GhostTrail::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	CheckNull(MeshComp);

	Character = Cast<ACharacter>(MeshComp->GetOwner());

	if (Character)
	{
		if (GhostTrailClass)
		{
			GhostTrail = ACGhostTrail::SpawnGhostTrail(Character, GhostTrailClass);
			GhostTrail->SetOwner(Cast<AActor>(Character));
			GhostTrail->ToggleOn();

			Character->GetWorldTimerManager().SetTimer(SpawnTimer, this, &UCAnimNotifyState_GhostTrail::SpawnTrail, GhostTrailSpawnTime, true);
		}
		else
			GLog->Log("Empty Ghost Trail Class!!");
	}
}

void UCAnimNotifyState_GhostTrail::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	CheckNull(MeshComp);

	if (Character)
	{
		if (GhostTrail)
		{
			GhostTrail->DeleteTrail();
			Character->GetWorldTimerManager().ClearTimer(SpawnTimer);
		}
	}
}

void UCAnimNotifyState_GhostTrail::SpawnTrail()
{
	if (Character)
	{
		if (GhostTrailClass)
		{
				GhostTrail = ACGhostTrail::SpawnGhostTrail(Character, GhostTrailClass);
				GhostTrail->SetOwner(Character);
				GhostTrail->ToggleOn();
		}
	}
}