/* MEMO: CAIController.cpp 코드 배치 정리 완료 */

#include "CAIController.h"
#include "Global.h"
#include "Player/CPlayer.h"
#include "Enemy/CEnemy_Rifle.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Enemy/CEnemy_Rifle.h"

ACAIController::ACAIController()
{
	/* create actor component */
	CHelpers::CreateActorComponent<UAIPerceptionComponent>(this, &Perception, "Perception");
	CHelpers::CreateActorComponent<UBlackboardComponent>(this, &Blackboard, "Blackboard");

	/* Sight 설정 */
	Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight");
	Sight->SightRadius = 1200.f;
	Sight->LoseSightRadius = 1200.f;
	Sight->PeripheralVisionAngleDegrees = 120;
	Sight->SetMaxAge(2);
	Sight->DetectionByAffiliation.bDetectEnemies = true;
	Sight->DetectionByAffiliation.bDetectNeutrals = false;
	Sight->DetectionByAffiliation.bDetectFriendlies = false;

	/* Perception 설정 */
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

	//InPawn을 Enemy로 cast 
	Enemy = Cast<ACEnemy_Rifle>(InPawn);
	//Enemy의 TeamId 설정
	//SetGenericTeamId(Enemy->GetTeamId());
	//Enemy의 BehavorTree를 통해 Blackboard에 할당
	UseBlackboard(Enemy->GetBehaviorTree()->BlackboardAsset, Blackboard);
	//Enemy 에서 쓸 Blackboard 세팅
	Enemy->SetBlackboard(Blackboard);
	//BehaviorTree 실행
	RunBehaviorTree(Enemy->GetBehaviorTree());
}

void ACAIController::OnUnPossess()
{
	Super::OnUnPossess();
}

void ACAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	//인식할 actor를 저장할 배열
	TArray<AActor *> actors;
	//배열에다가 Percepstion이 인식한 엑터 저장
	Perception->GetCurrentlyPerceivedActors(nullptr, actors);
	//혹시 모를 인식을 막기 위해 nullptr로 초기화
	ACPlayer* player = nullptr;
	//배열 actors for문 돌려서
	for (AActor* actor : actors)
	{
		//actor가 걸리면 player로 캐스트 (이거는 Enemy는 Player만 인식하면 되니까 이렇게 함)
		player = Cast<ACPlayer>(actor);
		//player 있으면 루프 종료 (앞에 !! 붙이는 이유는 혹시 모를 값이 리턴되서 잘못된 결과가 나올까봐 함)
		if (!!player)
			break;
	}
	//Blackboard에서 Player라는 이름의 키를 찾아 player를 할당
	Blackboard->SetValueAsObject("Player", player);
	Blackboard->SetValueAsEnum("State", (uint8)Enemy->GetCurrentEnemyStateType());
}