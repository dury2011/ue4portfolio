#include "Notify/CAnimNotify_GhostTrailOn.h"
#include "Global.h"

//#include "Component/CSkillComponent.h"

FString UCAnimNotify_GhostTrailOn::GetNotifyName_Implementation() const
{
	return FString("GhostTrailOn");
}

void UCAnimNotify_GhostTrailOn::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	ACharacter* character = Cast<ACharacter>(MeshComp->GetOwner());

	if (character)
	{
		//UCSkillComponent* skillComponent = CHelpers::GetComponent<UCSkillComponent>(character);
		//skillComponent->SpawnEffect();
	}		
}

