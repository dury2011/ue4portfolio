#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CItemBox.generated.h"

UCLASS()
class UE4POFOL_F_API ACItemBox : public AActor
{
	GENERATED_BODY()
private:
	UPROPERTY()
	class ACItem* Item;
	
public:
	UPROPERTY(EditAnywhere, Category = "Setting")
	TSubclassOf<class ACItem> HavingItemClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Setting")
	float Hp = 250.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USphereComponent* SphereCollision;

public:	
	ACItemBox();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnDamageEffect();
	
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnDestroyEffect();


private:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
};
