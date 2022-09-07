#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CAnimNotify_SpawnWeapon.generated.h"

UCLASS()
class UE4POFOL_F_API UCAnimNotify_SpawnWeapon : public UAnimNotify
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "Setting")
	TSubclassOf<class ACWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, Category = "Setting")
	FName SpawnSocketName;

	UPROPERTY(EditAnywhere, Category = "Setting")
	bool IsBossWeapon = false;

private:
	FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);
};
