#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CCrosshair.generated.h"

UENUM(BlueprintType)
enum class ECrosshairType : uint8
{
	Normal, PortalOrThrow, Max,
};

UCLASS()
class UE4POFOL_F_API ACCrosshair : public AActor
{
	GENERATED_BODY()

public:

private:

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class USphereComponent* SphereCollision;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	class UParticleSystemComponent* ParticleSystem;

	UPROPERTY()
	class ACPlayer* Player;

	UPROPERTY()
	class UCameraComponent* PlayerCamera;

	FHitResult HitResult;

	bool IsPlayerRelativeAllocated = false;
	
public:	
	ACCrosshair();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	static ACCrosshair* SpawnCrosshair(ACharacter* InSpawner, TSubclassOf<ACCrosshair> InCrosshairClass);
	virtual void DestroyCrosshair();

protected:
	void CreateLineTrace(ECrosshairType InType);

public:
	FORCEINLINE virtual FHitResult GetHitResult() { return HitResult; }
};
