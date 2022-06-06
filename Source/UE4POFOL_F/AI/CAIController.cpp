/* MEMO: CAIController.cpp �ڵ� ��ġ ���� �Ϸ� */

#include "CAIController.h"
#include "Global.h"
#include "Player/CPlayer.h"
#include "Enemy/CEnemy.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

ACAIController::ACAIController()
{
	/* create actor component */
	CHelpers::CreateActorComponent<UAIPerceptionComponent>(this, &Perception, "Perception");
	CHelpers::CreateActorComponent<UBlackboardComponent>(this, &Blackboard, "Blackboard");

	/* Sight ���� */
	Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight");
	Sight->SightRadius = 1200.f;
	Sight->LoseSightRadius = 1200.f;
	Sight->PeripheralVisionAngleDegrees = 120;
	Sight->SetMaxAge(2);
	Sight->DetectionByAffiliation.bDetectEnemies = true;
	Sight->DetectionByAffiliation.bDetectNeutrals = false;
	Sight->DetectionByAffiliation.bDetectFriendlies = false;

	/* Perception ���� */
	Perception->ConfigureSense(*Sight);
	Perception->SetDominantSense(*Sight->GetSenseImplementation());
}

void ACAIController::BeginPlay()
{
	Super::BeginPlay();

	Perception->OnPerceptionUpdated.AddDynamic(this, &ACAIController::OnPerceptionUpdated);
}

void ACAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	/* InPawn�� Enemy�� cast */
	Enemy = Cast<ACEnemy>(InPawn);
	/* Enemy�� TeamId ���� */
	//SetGenericTeamId(Enemy->GetTeamId());
	/* Enemy�� BehavorTree�� ���� Blackboard�� �Ҵ�*/
	UseBlackboard(Enemy->GetBehaviorTree()->BlackboardAsset, Blackboard);
	/* Enemy AIBehaviorComponent ���*/

	// Enemy ���� �� Blackboard ����
	Enemy->SetBlackboard(Blackboard);

	//AIStateComponent = CHelpers::GetComponent<UCAIStateComponent>(Enemy);
	/* AIBehaviourComponent�� Blackboard ���� */
	//AIStateComponent->SetBlackboard(Blackboard);
	/* BehaviorTree ���� */
	RunBehaviorTree(Enemy->GetBehaviorTree());
}

void ACAIController::OnUnPossess()
{
	Super::OnUnPossess();
}

void ACAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	/* �ν��� actor�� ������ �迭 */
	TArray<AActor *> actors;
	/* �迭���ٰ� Percepstion�� �ν��� ���� ���� */
	Perception->GetCurrentlyPerceivedActors(nullptr, actors);
	/* Ȥ�� �� �ν��� ���� ���� nullptr�� �ʱ�ȭ */
	ACPlayer* player = nullptr;
	/* �迭 actors for�� ������ */
	for (AActor* actor : actors)
	{
		/* actor�� �ɸ��� player�� ĳ��Ʈ (�̰Ŵ� Enemy�� Player�� �ν��ϸ� �Ǵϱ� �̷��� ��) */
		player = Cast<ACPlayer>(actor);
		/* player ������ ���� ���� (�տ� !! ���̴� ������ Ȥ�� �� ���� ���ϵǼ� �߸��� ����� ���ñ�� ��) */
		if (!!player)
			break;
	}
	/* Blackboard���� Player��� �̸��� Ű�� ã�� player�� �Ҵ� */
	Blackboard->SetValueAsObject("Player", player);
}