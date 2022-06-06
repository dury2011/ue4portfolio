/* MEMO: CIKComponent.h 코드 배치 정리 완료 */
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "CIKComponent.generated.h"

USTRUCT(BlueprintType)
struct FFeetData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Feet")
	FVector LeftDistance; //X

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Feet")
	FVector RightDistance; //X

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Feet")
	FVector PelvisDistance; //Z

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Feet")
	FRotator LeftRotation;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Feet")
	FRotator RightRotation;
};

USTRUCT(BlueprintType)
struct FArmData
{
	GENERATED_BODY()
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE4POFOL_F_API UCIKComponent : public UActorComponent
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category = "Trace")
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebug;

	UPROPERTY(EditAnywhere, Category = "Trace")
	float InterpSpeed = 50;

	UPROPERTY(EditAnywhere, Category = "Trace")
	float TraceDistance = 100;

	UPROPERTY(EditAnywhere, Category = "Trace")
	float OffsetDistance = 5;

	UPROPERTY(EditAnywhere, Category = "Socket")
	FName LeftSocket = "ik_foot_l";

	UPROPERTY(EditAnywhere, Category = "Socket")
	FName RightSocket = "ik_foot_r";

	class ACharacter* OwnerCharacter;

	FFeetData FeetData;

public:	
	UCIKComponent();

protected:
	virtual void BeginPlay() override;
	
private:
	void CheckIK(FName InName, float& OutDistance, FRotator& OutRotation);

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	FORCEINLINE FFeetData GetFeetData() { return FeetData; }
};
