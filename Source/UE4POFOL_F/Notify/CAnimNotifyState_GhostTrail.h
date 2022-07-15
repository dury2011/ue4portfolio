#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "CAnimNotifyState_GhostTrail.generated.h"

UCLASS()
class UE4POFOL_F_API UCAnimNotifyState_GhostTrail : public UAnimNotifyState
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "Setting")
	TSubclassOf<class ACGhostTrail> GhostTrailClass;

	UPROPERTY(EditAnywhere, Category = "Setting")
	float GhostTrailSpawnTime = 0.5f;

	class ACharacter* Character;
	class ACGhostTrail* GhostTrail;
	FTimerHandle SpawnTimer;

private:
	FString GetNotifyName_Implementation() const override;

	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation) override;

	void SpawnTrail();
};
