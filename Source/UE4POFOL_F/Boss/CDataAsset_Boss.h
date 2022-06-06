/* MEMO: CDataAsset_Boss.h 코드 배치 정리 완료 */
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Weapon/CWeaponStructure.h"
#include "CDataAsset_Boss.generated.h"

UCLASS()
class UE4POFOL_F_API UCDataAsset_Boss : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapon Class")
	TSubclassOf<class ACWeapon> WeaponClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "DoAction Class")
	TSubclassOf<class UCDoAction> DoActionClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attack Montage")
	TArray<FActionData> AttackMontagesAndDatas;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Hit Montage")
	TArray<FDamageData> DamageMontagesAndDatas;

	UPROPERTY()
	class ACBoss* Boss;

	UPROPERTY()
	class ACWeapon* Weapon;

	UPROPERTY()
	class UCDoAction* DoAction;
	
public:
	UCDataAsset_Boss();

	void BeginPlay(class ACharacter* InOwnerCharacter);

	FORCEINLINE ACBoss* GetBoss() { return Boss; }
	FORCEINLINE ACWeapon* GetWeapon() { return Weapon; }
	FORCEINLINE UCDoAction* GetDoAction() { return DoAction; }
	FORCEINLINE FDamageData GetDamageData(int InIndex) { return DamageMontagesAndDatas[InIndex]; }
	FORCEINLINE FActionData GetAttackMontagesAndDatas(int InIndex) { return AttackMontagesAndDatas[InIndex]; }
};
