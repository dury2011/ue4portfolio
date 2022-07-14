#include "Notify/CAnimNotifyState_RM_Enable_ZAxis.h"
#include "Global.h"
#include "GameFramework/Character.h"

FString UCAnimNotifyState_RM_Enable_ZAxis::GetNotifyName_Implementation() const
{
	return FString("Unlock RootMotion ZAxis");
}

void UCAnimNotifyState_RM_Enable_ZAxis::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
	CheckNull(MeshComp);
	
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	ACharacter* character = Cast<ACharacter>(MeshComp->GetOwner());

	if (character)
		character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
}

void UCAnimNotifyState_RM_Enable_ZAxis::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	CheckNull(MeshComp);
	

	ACharacter* character = Cast<ACharacter>(MeshComp->GetOwner());

	if (character)
		character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	
	Super::NotifyEnd(MeshComp, Animation);
}

