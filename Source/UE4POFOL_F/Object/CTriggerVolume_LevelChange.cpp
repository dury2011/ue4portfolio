#include "Object/CTriggerVolume_LevelChange.h"
#include "Global.h"

ACTriggerVolume_LevelChange::ACTriggerVolume_LevelChange()
{
	SetbCanTrigger(true);
}

void ACTriggerVolume_LevelChange::OnBeginOverlap(class AActor* OverlappedActor, class AActor* OtherActor)
{
	Super::OnBeginOverlap(OverlappedActor, OtherActor);

	if (OtherActor->ActorHasTag(FName("Player")))
		UGameplayStatics::OpenLevel(GetWorld(), FName("Level2_Boss"));
}

void ACTriggerVolume_LevelChange::OnEndOverlap(class AActor* OverlappedActor, class AActor* OtherActor)
{
	Super::OnEndOverlap(OverlappedActor, OtherActor);

	if (OtherActor->ActorHasTag(FName("Player")))
		SetbCanTrigger(false);
}