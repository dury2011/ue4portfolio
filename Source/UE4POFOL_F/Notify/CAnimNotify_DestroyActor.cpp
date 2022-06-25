#include "Notify/CAnimNotify_DestroyActor.h"
#include "Global.h"

FString UCAnimNotify_DestroyActor::GetNotifyName_Implementation() const
{
	return "Destory Actor";
}

void UCAnimNotify_DestroyActor::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	
	CheckNull(MeshComp);
	
	AActor* actor = MeshComp->GetOwner();

	if (actor)
		actor->Destroy();
}
