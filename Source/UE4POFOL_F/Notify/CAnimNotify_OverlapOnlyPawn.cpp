#include "Notify/CAnimNotify_OverlapOnlyPawn.h"
#include "Global.h"

#include "GameFramework/Character.h"

FString UCAnimNotify_OverlapOnlyPawn::GetNotifyName_Implementation() const
{
	return FString("CPN_OverlapOnlyPawn");
}

void UCAnimNotify_OverlapOnlyPawn::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACharacter* character = Cast<ACharacter>(MeshComp->GetOwner());

	if (!!character)
		character->GetCapsuleComponent()->SetCollisionProfileName(FName("OverlapOnlyPawn"));
}
