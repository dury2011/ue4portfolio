#include "Object/CTriggerVolume_LevelChange.h"
#include "Global.h"
#include "Interface/CInterface_PlayerState.h"

ACTriggerVolume_LevelChange::ACTriggerVolume_LevelChange()
{
	SetbCanTrigger(true);
}

void ACTriggerVolume_LevelChange::OnBeginOverlap(class AActor* OverlappedActor, class AActor* OtherActor)
{
	Super::OnBeginOverlap(OverlappedActor, OtherActor);

	FLatentActionInfo info;
	info.CallbackTarget = this;

	if (OtherActor->ActorHasTag(FName("Player")))
	{
		UGameplayStatics::UnloadStreamLevel(GetWorld(), FName("Level1"), info, false);
		UGameplayStatics::LoadStreamLevel(GetWorld(), FName("Level2_Boss"), true, false, info);
	}
}

void ACTriggerVolume_LevelChange::OnEndOverlap(class AActor* OverlappedActor, class AActor* OtherActor)
{
	Super::OnEndOverlap(OverlappedActor, OtherActor);

	if (OtherActor->ActorHasTag(FName("Player")))
		SetbCanTrigger(false);

	ICInterface_PlayerState* playerInterface = Cast<ICInterface_PlayerState>(OtherActor);

	if (playerInterface)
		playerInterface->SetPlayerIsInBossStage(true);
}