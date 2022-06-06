#include "Notify/CAnimNotify_Pawn.h"
#include "Global.h"

#include "GameFramework/Character.h"

FString UCAnimNotify_Pawn::GetNotifyName_Implementation() const
{
	return FString("CPN_Pawn");
}

void UCAnimNotify_Pawn::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);
	
	ACharacter* character = Cast<ACharacter>(MeshComp->GetOwner());

	if (!!character)
		character->GetCapsuleComponent()->SetCollisionProfileName(FName("Pawn"));
}
