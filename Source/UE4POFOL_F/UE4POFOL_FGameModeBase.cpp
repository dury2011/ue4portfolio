#include "UE4POFOL_FGameModeBase.h"
#include "Global.h"
#include "Object/CTriggerVolume_LevelChange.h"

AUE4POFOL_FGameModeBase::AUE4POFOL_FGameModeBase()
{
	CHelpers::GetClass<APawn>(&DefaultPawnClass, "Blueprint'/Game/FORUE4POFOL/Player/Blueprint/BP_CPlayer.BP_CPlayer_C'");
}

void AUE4POFOL_FGameModeBase::StartPlay()
{
	TArray<AActor*> outActorArr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACTriggerVolume_LevelChange::StaticClass(), outActorArr);

	for (int i = 0; i < outActorArr.Num(); i++)
		TriggerVolume = dynamic_cast<ACTriggerVolume_LevelChange*>(outActorArr[i]);
	
	Super::StartPlay();
}