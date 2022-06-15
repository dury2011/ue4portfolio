#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSpline.generated.h"

UCLASS()
class UE4POFOL_F_API ACSpline : public AActor
{
	GENERATED_BODY()
	
public:

private:

protected:
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Spline Setting")
	USceneComponent* Root;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Spline Setting")
	class USplineComponent* SplineComponent;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = "Spline Setting")
	TSubclassOf<class AActor> ActorToFollowClass;

	UPROPERTY()
	class AActor* EnemyToFollow;

	float StartTime = 0.0f;
	bool bCanMoveActor = false;
	
public:	
	ACSpline();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
