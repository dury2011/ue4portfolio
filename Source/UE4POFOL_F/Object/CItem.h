#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/CInterface_Item.h"
#include "CItem.generated.h"

UCLASS()
class UE4POFOL_F_API ACItem : public AActor, public ICInterface_Item
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Setting")
	EItemType ItemType = EItemType::Max;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AActor* EatActor;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USphereComponent* SphereCollision;
	
public:	
	ACItem();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	static ACItem* SpawnItem(AActor* InSpawner, TSubclassOf<class ACItem> InItemClass, const FTransform& InSpawnTransform);

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnDestroyEffect();
	
private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	virtual void SetCurrentItemType(EItemType InType) override;
	virtual EItemType GetCurrentItemType() override;
	virtual float ActivateItemAbility() override;
};
