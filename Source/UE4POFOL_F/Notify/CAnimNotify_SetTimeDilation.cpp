#include "Notify/CAnimNotify_SetTimeDilation.h"
#include "Global.h"
#include "GameFramework/Character.h"

FString UCAnimNotify_SetTimeDilation::GetNotifyName_Implementation() const
{
	return FString("Set Custom Time Dilation");
}

void UCAnimNotify_SetTimeDilation::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACharacter* character = Cast<ACharacter>(MeshComp->GetOwner());

	if (character)
		character->CustomTimeDilation = DilationTime;
}