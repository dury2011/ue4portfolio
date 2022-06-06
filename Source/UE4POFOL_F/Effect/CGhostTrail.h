/* MEMO: CGhostTrail.h �ڵ� ��ġ ���� �Ϸ� */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CGhostTrail.generated.h"

UCLASS()
class UE4POFOL_F_API ACGhostTrail : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY()
	class ACharacter* OwnerCharacter;

	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(VisibleDefaultsOnly)
	class UPoseableMeshComponent* Poseable;

	UPROPERTY(EditDefaultsOnly, Category = "Material")
	class UMaterialInstanceConstant* Material;

	UPROPERTY(EditDefaultsOnly, Category = "Time")
	float StartDelay = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Time")
	float Interval = 0.25f;
	
public:	
	ACGhostTrail();

protected:
	virtual void BeginPlay() override;

public:
	void SpawnGhostTrail(ACharacter* InCharacter);
	void ToggleOn();
	void ToggleOff();
	void DeleteTrail();
};