#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CGrenade.generated.h"

UCLASS()
class UE4POFOL_F_API ACGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	ACGrenade();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
