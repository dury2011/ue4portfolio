/* MEMO : ACAIController_Boss.cpp 코드 배치 정리 완료 */
#include "AI/CAIController_Boss.h"
#include "Global.h"
#include "Boss/CBoss.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Player/CPlayer.h"

ACAIController_Boss::ACAIController_Boss()
{
	PrimaryActorTick.bCanEverTick = true;
	
	CHelpers::CreateActorComponent<UBlackboardComponent>(this, &Blackboard, "Blackboard");
	CHelpers::CreateActorComponent<UAIPerceptionComponent>(this, &Perception_Sight, "Perception");

	Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight");
	Sight->SightRadius = 3600.f;
	Sight->LoseSightRadius = 3600.f;
	Sight->PeripheralVisionAngleDegrees = 360.f;
	Sight->SetMaxAge(2);
	Sight->DetectionByAffiliation.bDetectEnemies = true;
	Sight->DetectionByAffiliation.bDetectNeutrals = false;
	Sight->DetectionByAffiliation.bDetectFriendlies = false;

	Perception_Sight->ConfigureSense(*Sight);
	Perception_Sight->SetDominantSense(*Sight->GetSenseImplementation());
}

void ACAIController_Boss::BeginPlay()
{
	Super::BeginPlay();
	
	Perception_Sight->OnPerceptionUpdated.AddDynamic(this, &ACAIController_Boss::OnPerceptionUpdated);
}

void ACAIController_Boss::Tick(float DeltaTime)
{

}

void ACAIController_Boss::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	Boss = Cast<ACBoss>(InPawn);

	if(Boss->GetBehaviorTree())
		UseBlackboard(Boss->GetBehaviorTree()->BlackboardAsset, Blackboard);

	Boss->SetBlackboard(Blackboard);

	RunBehaviorTree(Boss->GetBehaviorTree());
}

void ACAIController_Boss::OnUnPossess()
{
	Super::OnUnPossess();
}

void ACAIController_Boss::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	TArray<AActor *> actors;

	Perception_Sight->GetCurrentlyPerceivedActors(nullptr, actors);

	ACPlayer* player = nullptr;

	for (AActor* actor : actors)
	{
		player = Cast<ACPlayer>(actor);

		if (!!player)
			break;
	}

	Blackboard->SetValueAsObject("Player", player);
}