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

	//UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Spline Setting")
	//TSubclassOf<class AActor> ActorToFollowClass;
	//
	//UPROPERTY()
	//class AActor* EnemyToFollow;
	//
	//float TotalPathTimeController = 10.0f;
	//float StartTime = 0.0f;
	//bool bCanMoveActor = false;
	//bool bSplineInLoop = true;
	
	int32 TargetSplinePointIndex;
	bool bReverse;
	float AcceptanceDistance = 150.0f;
public:	
	ACSpline();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	bool GetMoveToSplinePoint(FVector& OutLocation, float& OutAcceptanceDistance);
	void SetUpdateMoveToSplinePoint();
};
