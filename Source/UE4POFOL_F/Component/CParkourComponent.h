/* MEMO: CParkourComponent.h 코드 배치 정리 완료 */
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetSystemLibrary.h"
//#include "Player/CPlayer.h"
#include "CParkourComponent.generated.h"

/* 파쿠르 열거체 */
UENUM(BlueprintType)
enum class EParkourArrowType : uint8
{
	Center = 0, Ceil, Floor, Left, Right, Land, Max,
};

/* 파쿠를 타입을 위한 열거체 */
UENUM(BlueprintType)
enum class EParkourType : uint8
{
	Roll = 0, Slide, Jump, Dash, Hide, Flip, BeginClimbDown, BeginClimbUp, Climbing, EndClimbUp, EndClimbDown, EndClimbJump, HighFalling, Max,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnParkourTypeChanged, EParkourType, InNewType);

/* FTableRowBase를 상속하여 DataTable의 행 기준으로 사용가능 */
USTRUCT(BlueprintType)
struct FParkourData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	EParkourType Type;

	UPROPERTY(EditAnywhere)
	class UAnimMontage* Montage;

	UPROPERTY(EditAnywhere)
	float PlayRatio = 1;

	UPROPERTY(EditAnywhere)
	FName SectionName;

	UPROPERTY(EditAnywhere)
	float MinDistance;

	UPROPERTY(EditAnywhere)
	float MaxDistance;

	UPROPERTY(EditAnywhere)
	float Extent;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE4POFOL_F_API UCParkourComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnParkourTypeChanged OnParkourTypeChanged;

private:
	UPROPERTY()
	ACharacter* OwnerCharacter;

	UPROPERTY()
	class UArrowComponent* OwnerArrows[(int32)EParkourArrowType::Max];

	UPROPERTY()
	UAnimInstance* AnimInstance;
	
	UPROPERTY(EditAnywhere, Category = "Parkour DataTable")
	class UDataTable* ParkourDataTable;

	UPROPERTY(EditAnywhere, Category = "Parkour Setting")
	float ExecuteClimbObstacleHeight = 300.f;

	UPROPERTY(EditAnywhere, Category = "Parkour Setting")
	float LineTraceDistance = 600.f;

	UPROPERTY(EditAnywhere, Category = "Parkour Setting")
	float AngleOfExecuteParkour = 15.f;

	UPROPERTY(EditAnywhere, Category = "Parkour Setting")
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType;

	UPROPERTY(EditAnywhere, Category = "Parkour Setting")
	float FallingHighPlayDistance = 500.0f;
	
	UPROPERTY()
	class AActor* LineTraceHitActorCeil;
	
	UPROPERTY()
	class AActor* LineTraceHitActorCenter;
	
	UPROPERTY()
	class AActor* LineTraceHitActorLeft;
	
	UPROPERTY()
	class AActor* LineTraceHitActorRight;
	
	UPROPERTY()
	class AActor* LineTraceHitActorFloor;

	UPROPERTY()
	class AActor* LineTraceHitActorLand;

	UPROPERTY()
	class AActor* ParkourObstacle;

	TMap<EParkourType, TArray<FParkourData>> ParkourDataMap;
	FHitResult HitResults[(int32)EParkourType::Max];
	FVector HitActorExtentCeil;
	FVector HitActorExtentCenter;
	FVector HitActorExtentLeft;
	FVector HitActorExtentRight;
	FVector HitActorExtentFloor;
	FVector HitActorExtentLand;
	EParkourType ParkourType = EParkourType::Max;
	EParkourType PrevParkourType = EParkourType::Max;
	FVector MinBound;
	FVector MaxBound;
	float HitActorDistanceCeil;
	float HitActorDistanceLeft;
	float HitActorDistanceRight;
	float HitActorDistanceCenter;
	float HitActorDistanceFloor;
	float HitActorDistanceLand;	
	float ToFrontYaw;
	bool BisParkour;
	bool BcanHide;
	/* Parkour 에님 몽타주가 재생 중일 때
	* 다시 DoParkour()가 입력으로 인한 호출이 이루어지지 않도록 하는 변수 */
	bool bParkouring = false;
	/* Hide와 Climb은 Blendspace를 사용하므로
	* ABP_Player에서 bool로 포즈 블렌딩을 위한 변수 */
	bool bHideOrClimb = false;
	bool bBeginJump = false;

	bool Done_CheckFallingHeight = false;
	float FallingStartHeight = 0.0f;

public:
	UCParkourComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void CreateLineTrace(EParkourArrowType InType);
	void LineTraceCeil();
	void LineTraceCenter();
	void LineTraceLeftRight();
	void LineTraceFloor();
	void LineTraceLand();
public:
	void DoParkour();
	void SetIdleMode();
	void SetRollMode();
	void SetSlideMode();
	void SetJumpMode();
	void SetHideMode();
	void SetFlipMode();
	void SetBeginClimbUpMode();
	void SetBeginClimbDownMode();
	void SetClimbingMode();
	void SetEndClimbUpMode();
	void SetEndClimbDownMode();
	void SetEndClimbJumpMode();

private: 
	bool CheckRollMode();
	bool CheckSlideMode();
	bool CheckHideMode();
	bool CheckFlipMode();
	bool CheckBeginClimbUpMode();
	bool CheckBeginClimbDownMode();
	
	/* 낙하 시 낙하 높이에 따른 조건에 해당하는 상태 만들기 */
	void CheckParkourState(EParkourType InCurrentType);

	void BeginRoll();
	void EndRoll();

	void BeginSlide();
	void EndSlide();

	void Land();

public:
	void BeginJump();
	void EndJump();

private:
	void BeginHide();
	void EndHide();

	void BeginFlip();
	void EndFlip();

	void BeginClimbUp();
	void BeginClimbDown();
	void EndClimbUp();
	void EndClimbDown();
	void CompleteClimb();

public:
	void Climbing();
	void AbortClimb();

private:
	UFUNCTION()
	void EndParkour(UAnimMontage* InMontage, bool InIsInterrupted);

	void ChangeType(EParkourType InNewType);

public:
	FORCEINLINE EParkourType GetParkourMode() { return ParkourType; }
	FORCEINLINE bool GetbParkouring() { return bParkouring; }
	FORCEINLINE bool IsIdleMode() { return ParkourType == EParkourType::Max; }
	FORCEINLINE bool IsRollMode() { return ParkourType == EParkourType::Roll; }
	FORCEINLINE bool IsSlideMode() { return ParkourType == EParkourType::Slide; }
	FORCEINLINE bool IsJumpMode() { return ParkourType == EParkourType::Jump; }
	FORCEINLINE bool IsHideMode() { return ParkourType == EParkourType::Hide; }
	FORCEINLINE bool IsFlipMode() { return ParkourType == EParkourType::Flip; }
	FORCEINLINE bool IsBeginClimbUpMode() { return ParkourType == EParkourType::BeginClimbUp; }
	FORCEINLINE bool IsBeginClimbDownMode() { return ParkourType == EParkourType::BeginClimbDown; }
	FORCEINLINE bool IsClimbingMode() { return ParkourType == EParkourType::Climbing; }
	FORCEINLINE bool IsEndClimbUpMode() { return ParkourType == EParkourType::EndClimbUp; }
	FORCEINLINE bool IsEndClimbDownMode() { return ParkourType == EParkourType::EndClimbDown; }
	FORCEINLINE bool IsEndClimbJumpMode() { return ParkourType == EParkourType::EndClimbJump; }
	FORCEINLINE bool GetbHideOrClimb() { return bHideOrClimb; }
	FORCEINLINE bool GetbBeginJump() { return bBeginJump; }
	/* Climb 모드는 에님 블루프린트에서 다루어야 한다.
		* BeginClimb 몽타주가 끝날 때 쯤에 bHideOrClimb을 true로 바꾸어
		* 바로 Climbing 모드로 들어가게 해주면된다.
		* 그렇게 하지 않으면 BeginClimb 몽타주가 끝나고
		* 바로 Climbing 모드로 넘어가지 않고, bool로 포즈 블렌딩에서
		* 중간에 Idle 포즈로 잠깐 연결되었다가 Climbing 모드로 넘어간다.
		* 노티파이에 이 함수를 이용하여 원하는 시점에 true로 바꾸면
		* 몽타주가 끝나기 전에 이미 true로 바뀌었기 때문에
		* Idle로 넘어가지 않고 바로 Climbing 모드로 넘어갈 것이다.
		* 다시 봤는데 이거는 전혀 상관없음 BeginClimb 모두 후
		* 적정 시간이 지나면 바로 Climbing 모드로 가면됨 */
	FORCEINLINE void SetbHideOrClimb(bool Inbool) { bHideOrClimb = Inbool; }
};
