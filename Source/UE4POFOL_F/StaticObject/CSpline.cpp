#include "StaticObject/CSpline.h"
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
	SplineComponent->Duration = 10.0f;
	SplineComponent->bDrawDebug = true;

}

void ACSpline::BeginPlay()
{
	Super::BeginPlay();	

	//TODO: Trigger Volume 구현하기
	//TArray<AActor*> outActorArr;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACEnemy_Rifle::StaticClass(), outActorArr);

	//for (int i = 0; i < outActorArr.Num(); i++)
	//	EnemyToFollows.Add(dynamic_cast<ACEnemy_Rifle*>(outActorArr[i]));
	if (ActorToFollowClass != nullptr)
	{
		EnemyToFollow = GetWorld()->SpawnActor<AActor>(ActorToFollowClass, FVector(/*TODO: 위치 설정*/));

		if (EnemyToFollow != nullptr)
		{
			StartTime = GetWorld()->GetTimeSeconds();
			bCanMoveActor = true;
		}
	}
	
}

void ACSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



	
}

