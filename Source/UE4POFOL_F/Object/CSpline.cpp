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

	//TODO: Trigger Volume �����ϱ�
	//TArray<AActor*> outActorArr;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACEnemy_Rifle::StaticClass(), outActorArr);
	//
	//for (int i = 0; i < outActorArr.Num(); i++)
	//	EnemyToFollows.Add(dynamic_cast<ACEnemy_Rifle*>(outActorArr[i]));
	//if (ActorToFollowClass != nullptr)
	//{
	//	//TODO: SpawnActor�� ��ġ ������ �ٽ� �ϱ� 
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

// ���ö��� �󿡼� �̵��� ����Ʈ�� ������Ʈ ���ִ� �Լ�
void ACSpline::SetUpdateMoveToSplinePoint()
{
	int32 maxSplinePointCount = SplineComponent->GetNumberOfSplinePoints();
	
	// ���������� ���� ��� 
	if (bReverse)
	{
		// ���Ͱ� ������ ���ö��� ����Ʈ �ε����� 0���� ũ��
		if (TargetSplinePointIndex > 0)
		{
			// ���� ���Ͱ� ������ ���ö��� ����Ʈ �ε����� �ٷ� ���� �ε��� 
			TargetSplinePointIndex--;

			// �Լ� ���� 
			return;
		}

		// ���ö��� ����Ʈ�� ���� �����̸� 
		if (SplineComponent->IsClosedLoop())
		{
			// ���� ���Ͱ� ������ ���ö��� ����Ʈ �ε����� ������ �ε��� 
			TargetSplinePointIndex = maxSplinePointCount - 1;

			// �Լ� ���� 
			return;
		}

		// ���� ���ö��� ����Ʈ �ε����� 0�̰�, ������ �ƴϸ� 
		TargetSplinePointIndex = 1;
		bReverse = false;
	}
	// ���������� ���� ��� 
	else
	{
		// ���� ���Ͱ� ������ ���ö����� ����Ʈ �ε����� ������ �ε����� �������� �ʾҴٸ� 
		if (TargetSplinePointIndex < maxSplinePointCount - 1)
		{
			// ���� ���Ͱ� ������ ���ö��� ����Ʈ�� �ٷ� ���� �ε��� 
			TargetSplinePointIndex++;

			// �Լ� ����
			return;
		}

		// ���� ���ö����� �������·� �����ִٸ�
		if (SplineComponent->IsClosedLoop())
		{
			// ���� ���Ͱ� �̵��� ���ö��� ����Ʈ �ε����� 0
			TargetSplinePointIndex = 0;

			// �Լ� ���� 
			return;
		}

		// ���� ���Ͱ� ������ �ε����� �����߰�, ������ �ƴϸ� 
		TargetSplinePointIndex = maxSplinePointCount - 2;
		bReverse = true;
	}
}


