#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPortalDoor.generated.h"

UCLASS()
class UE4POFOL_F_API ACPortalDoor : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USphereComponent* SphereCollision;

private:
	
public:	
	ACPortalDoor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	static ACPortalDoor* SpawnPortalDoor(ACharacter* InSpawner, const FVector& InSpawnLocation, TSubclassOf<class ACPortalDoor> InPortalDoorClass);
	void DestroyPortalDoor();

};
