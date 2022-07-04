#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CWeaponStructure.generated.h"

USTRUCT(BlueprintType)
struct FActionData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	class UAnimMontage* Montage;

	UPROPERTY(EditAnywhere)
	float PlayRatio = 1;

	UPROPERTY(EditAnywhere)
	FName SectionName;

	UPROPERTY(EditAnywhere)
	bool bCanMove = true;

	UPROPERTY(EditAnywhere)
	bool bFixedCamera;

	//UPROPERTY(EditAnywhere)
	//class UFXSystemAsset* Effect;
	
	//UPROPERTY(EditAnywhere)
	//FTransform EffectTransform;
	//
	//UPROPERTY(EditAnywhere)
	//class USoundCue* SoundCue;
	//
	//// Do not use in Dataasset.
	//UPROPERTY(EditAnywhere)
	//TSubclassOf<class ACGhostTrail> GhostTrailClass;
	void PlayMontage(class ACharacter* InOwnerCharacter);
	void StopMontage(class ACharacter* InOwnerCharacter);
	void PauseMontage(class ACharacter* InOnwerCharacter, class UAnimInstance* InAnimInstance);
	void ResumeMontage(class ACharacter* InOwnerCharacter, class UAnimInstance* InAnimInstance);
	void EndAction(class ACharacter* InOwnerCharacter);
};

USTRUCT(BlueprintType)
struct FDamageData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	class UAnimMontage* Montage;

	UPROPERTY(EditAnywhere)
	float PlayRatio = 1;

	UPROPERTY(EditAnywhere)
	bool bCanMove = false;

	UPROPERTY(EditAnywhere)
	float Power = 5;

	UPROPERTY(EditAnywhere)
	float Launch = 100;

	UPROPERTY(EditAnywhere)
	float StopTime;

	UPROPERTY(EditAnywhere)
	class UFXSystemAsset* Effect;
	
	//UPROPERTY(EditAnywhere)
	//FTransform EffectTransform;
	
	//UPROPERTY(EditAnywhere)
	//class USoundCue* SoundCue;
public:
	void PlayEffect(class UWorld* InWorld, class ACharacter* InOwner);
	void PlayHitStop(class UWorld* InWorld);
	void PlaySoundCue(class UWorld* InWorld, const FVector& InLocation);
	void SendDamage(class ACharacter* InAttacker, class AActor* InCauser, class ACharacter* InOtherCharacter);
	void PlayMontage(class ACharacter* InOwner);
	void PauseMontage(class ACharacter* InOnwerCharacter, class UAnimInstance* InAnimInstance);
	void ResumeMontage(class ACharacter* InOwnerCharacter, class UAnimInstance* InAnimInstance);
	void AirborneSkill(class ACharacter* InAirborneCharacter);
};

USTRUCT(BlueprintType)
struct FActionDamageEvent : public FDamageEvent
{
	GENERATED_BODY()

public:
	 FDamageData* DamageData;
};

UCLASS()
class UE4POFOL_F_API UCWeaponStructure : public UObject
{
	GENERATED_BODY()

public:
	UCWeaponStructure();

//public:
//FORCEINLINE static class UAnimMontage* GetDefaultHitMontage() { return DefaultHitMontage; }
//
//private:
//static class UAnimMontage* DefaultHitMontage;
};

