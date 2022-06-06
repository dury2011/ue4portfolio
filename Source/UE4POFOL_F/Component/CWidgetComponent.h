#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CWidgetComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE4POFOL_F_API UCWidgetComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	ACharacter* OwnerCharacter;

	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY()
	class UPoseableMeshComponent* Poseable;

	//UPROPERTY(EditDefaultsOnly, Category = "Debug Setting")
	//TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType;
	
	FHitResult HitResult;

	bool bCrosshairVisible = true;

public:	
	UCWidgetComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	void CreateLineTrace();

public:
	UFUNCTION(BlueprintCallable)
	bool CanCrosshairWidgetColorChange();

	FORCEINLINE void SetbCrosshairVisible(bool Inbool) { bCrosshairVisible = Inbool; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetbCrosshairVisible() { return bCrosshairVisible; }

	FORCEINLINE FHitResult GetHitResult() { return HitResult; }
};
