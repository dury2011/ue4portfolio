#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CAnimNotify_Launch.generated.h"

UENUM(BlueprintType)
enum class ECharacterLaunchDirectionType : uint8
{
	Foward, Backward, Left, Right
};
UCLASS()
class UE4POFOL_F_API UCAnimNotify_Launch : public UAnimNotify
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	ECharacterLaunchDirectionType LaunchDirectionType;

	UPROPERTY(EditAnywhere)
	float LaunchLength = 100.0f;

	UPROPERTY(EditAnywhere)
	bool XYOverlap = true;

	UPROPERTY(EditAnywhere)
	bool ZOverlap = true;
	
private:
	FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);
};
