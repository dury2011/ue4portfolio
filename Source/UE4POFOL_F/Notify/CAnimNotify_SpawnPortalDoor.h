#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CAnimNotify_SpawnPortalDoor.generated.h"

UCLASS()
class UE4POFOL_F_API UCAnimNotify_SpawnPortalDoor : public UAnimNotify
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "Setting")
	TSubclassOf<class ACPortalDoor> PortalDoorClass;

	UPROPERTY(EditAnywhere, Category = "Setting")
	FName SpawnSocketName;
	
private:
	FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);
};
