#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CAnimNotify_SpawnEnemy.generated.h"

UCLASS()
class UE4POFOL_F_API UCAnimNotify_SpawnEnemy : public UAnimNotify
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<ACEnemy> SpawnEnemyClass;

	UPROPERTY(EditAnywhere)
	FName SpawnSocketName;

private:
	FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation);	
};
