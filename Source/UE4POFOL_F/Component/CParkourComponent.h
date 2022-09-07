/* MEMO: CParkourComponent.h �ڵ� ��ġ ���� �Ϸ� */
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetSystemLibrary.h"
//#include "Player/CPlayer.h"
#include "CParkourComponent.generated.h"

/* ���� ����ü */
UENUM(BlueprintType)
enum class EParkourArrowType : uint8
{
	Center = 0, Ceil, Floor, Left, Right, Land, Max,
};

/* ���� Ÿ���� ���� ����ü */
UENUM(BlueprintType)
enum class EParkourType : uint8
{
	Roll = 0, Slide, Jump, Dash, Hide, Flip, BeginClimbDown, BeginClimbUp, Climbing, EndClimbUp, EndClimbDown, EndClimbJump, HighFalling, Max,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnParkourTypeChanged, EParkourType, InNewType);

/* FTableRowBase�� ����Ͽ� DataTable�� �� �������� ��밡�� */
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
	/* Parkour ���� ��Ÿ�ְ� ��� ���� ��
	* �ٽ� DoParkour()�� �Է����� ���� ȣ���� �̷������ �ʵ��� �ϴ� ���� */
	bool bParkouring = false;
	/* Hide�� Climb�� Blendspace�� ����ϹǷ�
	* ABP_Player���� bool�� ���� ������ ���� ���� */
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
	
	/* ���� �� ���� ���̿� ���� ���ǿ� �ش��ϴ� ���� ����� */
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
	/* Climb ���� ���� �������Ʈ���� �ٷ��� �Ѵ�.
		* BeginClimb ��Ÿ�ְ� ���� �� �뿡 bHideOrClimb�� true�� �ٲپ�
		* �ٷ� Climbing ���� ���� ���ָ�ȴ�.
		* �׷��� ���� ������ BeginClimb ��Ÿ�ְ� ������
		* �ٷ� Climbing ���� �Ѿ�� �ʰ�, bool�� ���� ��������
		* �߰��� Idle ����� ��� ����Ǿ��ٰ� Climbing ���� �Ѿ��.
		* ��Ƽ���̿� �� �Լ��� �̿��Ͽ� ���ϴ� ������ true�� �ٲٸ�
		* ��Ÿ�ְ� ������ ���� �̹� true�� �ٲ���� ������
		* Idle�� �Ѿ�� �ʰ� �ٷ� Climbing ���� �Ѿ ���̴�.
		* �ٽ� �ôµ� �̰Ŵ� ���� ������� BeginClimb ��� ��
		* ���� �ð��� ������ �ٷ� Climbing ���� ����� */
	FORCEINLINE void SetbHideOrClimb(bool Inbool) { bHideOrClimb = Inbool; }
};
