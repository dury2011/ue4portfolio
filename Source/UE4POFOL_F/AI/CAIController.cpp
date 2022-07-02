/* MEMO: CAIController.cpp �ڵ� ��ġ ���� �Ϸ� */

#include "CAIController.h"
#include "Global.h"
#include "Player/CPlayer.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
//#include "Enemy/CEnemy_Rifle.h"
//#include "GameFramework/Character.h"
//#include "Perception/AIPerceptionComponent.h"
//#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Enemy/CEnemy.h"
#include "Component/CCharacterComponent.h"


ACAIController::ACAIController()
{
	// create actor component
	//CHelpers::CreateActorComponent<UAIPerceptionComponent>(this, &Perception, "Perception");
	//CHelpers::CreateActorComponent<UBlackboardComponent>(this, &Blackboard, "Blackboard");
	//
	//Sight ����
	//Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight");
	//Sight->SightRadius = 1000.0f;
	//Sight->LoseSightRadius = 1000.0f;
	//Sight->PeripheralVisionAngleDegrees = 120;
	//Sight->SetMaxAge(2);
	//Sight->DetectionByAffiliation.bDetectEnemies = true;
	//Sight->DetectionByAffiliation.bDetectNeutrals = false;
	//Sight->DetectionByAffiliation.bDetectFriendlies = false;
	//
	//Perception ����
	//Perception->ConfigureSense(*Sight);
	//Perception->SetDominantSense(*Sight->GetSenseImplementation());
}

void ACAIController::BeginPlay()
{
	Super::BeginPlay();

	//Perception->OnPerceptionUpdated.AddDynamic(this, &ACAIController::OnPerceptionUpdated);
}

void ACAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (Enemy)
	{
		//if (Enemy->GetOpponent())
		//	Blackboard->SetValueAsObject("Player", Enemy->GetOpponent());
	
		Blackboard->SetValueAsEnum("State", (uint8)Enemy->GetCurrentEnemyStateType());		
	}
}

void ACAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	//GetWorld()->GetTimerManager().SetTimer(Timer, this, &ACAIController::OnRepeatTimer, 2.0f, true);
	Enemy = Cast<ACEnemy>(InPawn);

	
	if (BehaviorTree)
	{	
		if(UseBlackboard(BehaviorTree->BlackboardAsset, Blackboard))
			RunBehaviorTree(BehaviorTree);

		Blackboard->SetValueAsVector("SpawnLocation", InPawn->GetActorLocation());

		//if (Enemy->GetOpponent())
		//	Blackboard->SetValueAsObject("Player", Enemy->GetOpponent());
	}
	
	//TODO: �Ʒ� �ڵ� �ּ� �ǳʶٴ� �κ� ������, Spawn�Ǵ� Actor������ ������ �ϴµ� Play���� Viewport�� Placed�� Actor������ ������ �� ��
	//TArray<AActor*> outActorArr;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPlayer::StaticClass(), outActorArr);
	//
	//for (int i = 0; i < outActorArr.Num(); i++)
	//	Player = dynamic_cast<ACPlayer*>(outActorArr[i]);
	//
	//if (Player)
	//{
	//	Enemy = Cast<ACEnemy>(InPawn);
	//	Enemy->SetOpponent(dynamic_cast<ACharacter*>(Player));
	//}
	//
	////InPawn�� Enemy�� cast 
	////Enemy�� TeamId ����
	////SetGenericTeamId(Enemy->GetTeamId());
	////Enemy�� BehavorTree�� ���� Blackboard�� �Ҵ�
	//UseBlackboard(Enemy->GetBehaviorTree()->BlackboardAsset, Blackboard);
	////Enemy ���� �� Blackboard ����
	//Enemy->SetBlackboard(Blackboard);
	////BehaviorTree ����
	//RunBehaviorTree(Enemy->GetBehaviorTree());
}

void ACAIController::OnUnPossess()
{
	Super::OnUnPossess();

	//GetWorld()->GetTimerManager().ClearTimer(Timer);
}

//void ACAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
//{
	////�ν��� actor�� ������ �迭
	//TArray<AActor *> actors;
	////�迭���ٰ� Percepstion�� �ν��� ���� ����
	//Perception->GetCurrentlyPerceivedActors(nullptr, actors);
	////Ȥ�� �� �ν��� ���� ���� nullptr�� �ʱ�ȭ
	//ACPlayer* player = nullptr;
	////�迭 actors for�� ������
	//for (AActor* actor : actors)
	//{
	//	//actor�� �ɸ��� player�� ĳ��Ʈ (�̰Ŵ� Enemy�� Player�� �ν��ϸ� �Ǵϱ� �̷��� ��)
	//	player = Cast<ACPlayer>(actor);
	//	//player ������ ���� ���� (�տ� !! ���̴� ������ Ȥ�� �� ���� ���ϵǼ� �߸��� ����� ���ñ�� ��)
	//	if (!!player)
	//		break;
	//}
	////Blackboard���� Player��� �̸��� Ű�� ã�� player�� �Ҵ�
	//Blackboard->SetValueAsObject("Player", player);
	//Blackboard->SetValueAsEnum("State", (uint8)Enemy->GetCurrentEnemyStateType());
//}

void ACAIController::OnRepeatTimer()
{
	//auto currentPawn = GetPawn();
	//
	//if (currentPawn == nullptr)
	//	return;

	//UNavigationSystemV1* navSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());

	//if (navSystem == nullptr)
	//	return;

	//FNavLocation nextLocation;

	//if (navSystem->GetRandomPointInNavigableRadius(FVector::ZeroVector, 500.0f, nextLocation))
	//{
	//	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, nextLocation.Location);
	//}
}