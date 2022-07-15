#include "Notify/CAnimNotify_LaunchUp.h"
#include "Global.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


FString UCAnimNotify_LaunchUp::GetNotifyName_Implementation() const
{
	return FString("LaunchUp");
}

void UCAnimNotify_LaunchUp::Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
	CheckNull(MeshComp);
	ACharacter* character = Cast<ACharacter>(MeshComp->GetOwner());

	if (character)
		character->LaunchCharacter(FVector(0.0f, 0.0f, LaunchDistance), true, true);
}

