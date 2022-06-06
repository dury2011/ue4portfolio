#include "UE4POFOL_FGameModeBase.h"
#include "Global.h"

AUE4POFOL_FGameModeBase::AUE4POFOL_FGameModeBase()
{
	CHelpers::GetClass<APawn>(&DefaultPawnClass, "Blueprint'/Game/FORUE4POFOL/Player/BP_CPlayer.BP_CPlayer_C'");
}