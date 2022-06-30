#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPortalCrosshair.generated.h"

UCLASS()
class UE4POFOL_F_API ACPortalCrosshair : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY()
	class ACPlayer* Player;
	
	UPROPERTY(EditDefaultsOnly)
	class USphereComponent* SphereCollision;
	
	//UPROPERTY()
	//class UParticleSystem* ParticleSystem;

	FHitResult HitResult;
	
public:	
	ACPortalCrosshair();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	void CreateLineTrace();

public:
	static ACPortalCrosshair* SpawnPortalCrosshair(ACharacter* InSpawner, TSubclassOf<class ACPortalCrosshair> InPortalCrosshairClass);

	void DestroyPortalCrosshair();

	FORCEINLINE FHitResult GetHitResult() { return HitResult; }
};
