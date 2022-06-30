#include "Object/CSpline.h"
#include "Global.h"
#include "Components/SplineComponent.h"
#include "Enemy/CEnemy_Rifle.h"

ACSpline::ACSpline()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SplineComponent->SetupAttachment(Root);
	SplineComponent->bDrawDebug = true;
	//SplineComponent->Duration = TotalPathTimeController;

	TargetSplinePointIndex = 0;
}

void ACSpline::BeginPlay()
{
	Super::BeginPlay();	

	//TODO: Trigger Volume 구현하기
	//TArray<AActor*> outActorArr;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACEnemy_Rifle::StaticClass(), outActorArr);
	//
	//for (int i = 0; i < outActorArr.Num(); i++)
	//	EnemyToFollows.Add(dynamic_cast<ACEnemy_Rifle*>(outActorArr[i]));
	//if (ActorToFollowClass != nullptr)
	//{
	//	//TODO: SpawnActor의 위치 설정은 다시 하기 
	//	EnemyToFollow = GetWorld()->SpawnActor<AActor>(ActorToFollowClass, SplineComponent->GetComponentTransform());
	//
	//	if (EnemyToFollow != nullptr)
	//	{
	//		StartTime = GetWorld()->GetTimeSeconds();
	//		bCanMoveActor = true;
	//	}
	//}
}

void ACSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if ((EnemyToFollow != nullptr) && (bCanMoveActor))
	//{
	//	float currentSplineTime = (GetWorld()->GetTimeSeconds() - StartTime) / TotalPathTimeController;
	//	float distance = SplineComponent->GetSplineLength() * currentSplineTime;
	//
	//	FVector position = SplineComponent->GetLocationAtDistanceAlongSpline(distance, ESplineCoordinateSpace::World);
	//	EnemyToFollow->SetActorLocation(position);
	//
	//	FVector direction = SplineComponent->GetLocationAtDistanceAlongSpline(distance, ESplineCoordinateSpace::World);
	//	FRotator rotator = FRotationMatrix::MakeFromX(direction).Rotator();
	//	EnemyToFollow->SetActorRotation(rotator);
	//
	//	if (currentSplineTime >= 1.0f)
	//	{
	//		if (bSplineInLoop)
	//		{
	//			bCanMoveActor = true;
	//			StartTime = GetWorld()->GetTimeSeconds();
	//			currentSplineTime = (GetWorld()->GetTimeSeconds() - StartTime) / TotalPathTimeController;
	//		}
	//	}
	//}
	//
	// int32 count = SplineComponent->GetNumberOfSplinePoints();
}

bool ACSpline::GetMoveToSplinePoint(FVector& OutLocation, float& OutAcceptanceDistance)
{
	OutLocation = FVector::ZeroVector;
	OutAcceptanceDistance = 0.0f;
	OutLocation = SplineComponent->GetLocationAtSplinePoint(TargetSplinePointIndex, ESplineCoordinateSpace::World);
	OutAcceptanceDistance = AcceptanceDistance;

	return true;
}

// 스플라인 상에서 이동할 포인트를 업데이트 해주는 함수
void ACSpline::SetUpdateMoveToSplinePoint()
{
	int32 maxSplinePointCount = SplineComponent->GetNumberOfSplinePoints();
	
	// 역방향으로 가는 경우 
	if (bReverse)
	{
		// 엑터가 도달한 스플라인 포인트 인덱스가 0보다 크면
		if (TargetSplinePointIndex > 0)
		{
			// 현재 엑터가 도달할 스플라인 포인트 인덱스는 바로 이전 인덱스 
			TargetSplinePointIndex--;

			// 함수 종료 
			return;
		}

		// 스플라인 포인트가 루프 형태이면 
		if (SplineComponent->IsClosedLoop())
		{
			// 현재 엑터가 도달할 스플라인 포인트 인덱스는 마지막 인덱스 
			TargetSplinePointIndex = maxSplinePointCount - 1;

			// 함수 종료 
			return;
		}

		// 현재 스플라인 포인트 인덱스가 0이고, 루프도 아니면 
		TargetSplinePointIndex = 1;
		bReverse = false;
	}
	// 정방향으로 가는 경우 
	else
	{
		// 현재 엑터가 도달한 스플라인의 포인트 인덱스가 마지막 인덱스에 도달하지 않았다면 
		if (TargetSplinePointIndex < maxSplinePointCount - 1)
		{
			// 현재 엑터가 도달할 스플라인 포인트는 바로 다음 인덱스 
			TargetSplinePointIndex++;

			// 함수 종료
			return;
		}

		// 현재 스플라인이 루프형태로 막혀있다면
		if (SplineComponent->IsClosedLoop())
		{
			// 현재 엑터가 이동할 스플라인 포인트 인덱스는 0
			TargetSplinePointIndex = 0;

			// 함수 종료 
			return;
		}

		// 현재 엑터가 마지막 인덱스에 도달했고, 루프도 아니면 
		TargetSplinePointIndex = maxSplinePointCount - 2;
		bReverse = true;
	}
}


