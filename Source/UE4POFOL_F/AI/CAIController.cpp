/* MEMO: CAIController.cpp 코드 배치 정리 완료 */

#include "CAIController.h"
#include "Global.h"
#include "Player/CPlayer.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
//#include "Enemy/CEnemy_Rifle.h"
//#include "GameFramework/Character.h"
//#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Enemy/CEnemy.h"
#include "Component/CCharacterComponent.h"
#include "Player/CCannon.h"

ACAIController::ACAIController(const FObjectInitializer& ObjectInitializer)
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("Perception");

	//Sight 설정
	Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight");

	//Perception 설정
	AIPerceptionComponent->ConfigureSense(*Sight);
	AIPerceptionComponent->SetDominantSense(*Sight->GetSenseImplementation());
}

void ACAIController::BeginPlay()
{
	Super::BeginPlay();

	ACEnemy* enemy = Cast<ACEnemy>(GetPawn());

	if (enemy)
	{
		Enemy = enemy;
		TeamId = FGenericTeamId(Enemy->ID);
	}

}

void ACAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (Enemy)
	{
		Blackboard->SetValueAsEnum("State", (uint8)Enemy->GetCurrentEnemyStateType());
		//Blackboard->SetValueAsObject("Opponent", Enemy->GetOpponent());

		if (Enemy->GetOpponent())
		{
			ICInterface_PlayerState* interfacePlayer = Cast<ICInterface_PlayerState>(Enemy->GetOpponent());
			
			if (interfacePlayer)
			{
				if (interfacePlayer->GetPlayerActivateSkill() && !IsSkillActivate)
				{
					IsSkillActivate = true;

					BrainComponent->StopLogic(FString("Enemy Skill Damage"));
				}
				else if (!interfacePlayer->GetPlayerActivateSkill() && IsSkillActivate)
				{
					IsSkillActivate = false;
					
					BrainComponent->RestartLogic();
				}
			}
		}
	}
}

void ACAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	//GetWorld()->GetTimerManager().SetTimer(Timer, this, &ACAIController::OnRepeatTimer, 2.0f, true);
	Enemy = Cast<ACEnemy>(InPawn);

	Enemy->OnEnemyDiedStopAI.AddDynamic(this, &ACAIController::StopEnemyAI);
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ACAIController::OnPerception);

	if (BehaviorTree)
	{	
		if(UseBlackboard(BehaviorTree->BlackboardAsset, Blackboard))
			RunBehaviorTree(BehaviorTree);

		Blackboard->SetValueAsVector("SpawnLocation", InPawn->GetActorLocation());

		//if (Enemy->GetOpponent())
		//	Blackboard->SetValueAsObject("Player", Enemy->GetOpponent());
	}

	IsSkillActivate = false;
	
	//TODO: 아래 코드 주석 건너뛰는 부분 전까지, Spawn되는 Actor에서는 동작을 하는데 Play전에 Viewport에 Placed된 Actor에서는 동작을 안 함
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
	////InPawn을 Enemy로 cast 
	////Enemy의 TeamId 설정
	////SetGenericTeamId(Enemy->GetTeamId());
	////Enemy의 BehavorTree를 통해 Blackboard에 할당
	//UseBlackboard(Enemy->GetBehaviorTree()->BlackboardAsset, Blackboard);
	////Enemy 에서 쓸 Blackboard 세팅
	//Enemy->SetBlackboard(Blackboard);
	////BehaviorTree 실행
	//RunBehaviorTree(Enemy->GetBehaviorTree());
}

void ACAIController::OnUnPossess()
{
	Super::OnUnPossess();

	//GetWorld()->GetTimerManager().ClearTimer(Timer);
}

//void ACAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
//{
//	//인식할 actor를 저장할 배열
//	TArray<AActor *> actors;
//	//배열에다가 Percepstion이 인식한 엑터 저장
//	PerceptionComponent->GetCurrentlyPerceivedActors(nullptr, actors);
//	//혹시 모를 인식을 막기 위해 nullptr로 초기화
//	ACPlayer* player = nullptr;
//	//배열 actors for문 돌려서
//	for (AActor* actor : actors)
//	{
//		//actor가 걸리면 player로 캐스트 (이거는 Enemy는 Player만 인식하면 되니까 이렇게 함)
//		player = Cast<ACPlayer>(actor);
//		//player 있으면 루프 종료 (앞에 !! 붙이는 이유는 혹시 모를 값이 리턴되서 잘못된 결과가 나올까봐 함)
//		if (!!player)
//			break;
//	}
//	//Blackboard에서 Player라는 이름의 키를 찾아 player를 할당
//	Blackboard->SetValueAsObject("Player", player);
//	Blackboard->SetValueAsEnum("State", (uint8)Enemy->GetCurrentEnemyStateType());
//}


void ACAIController::StopEnemyAI()
{
	BrainComponent->StopLogic(FString("Enemy Died"));
	SetActorTickEnabled(false);
}

/* https://www.youtube.com/watch?v=HwBdnviT2vs 
 * 아래 함수에서 리턴값은 어디서 어떻게 사용되는건가? */

ETeamAttitude::Type ACAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	//시야에 들어온 엑터가 폰 이상인지 확인
	const APawn* OtherPawn = Cast<APawn>(&Other);
	if(OtherPawn == nullptr)
	{
		return ETeamAttitude::Neutral;
	}

	//엑터가 IGenericTeamAgentInterface를 상속받았는가 확인
	// Player는 AIController가 없으므로 
	//auto PlayerTI = Cast <IGenericTeamAgentInterface>(&Other);
	//const ACPlayer* player = Cast<ACPlayer>(&Other);

	// Bot(Enemy)는 AIContorller가 있으므로 
	class IGenericTeamAgentInterface* BotTI = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController());
	
	if(BotTI == nullptr /*&& PlayerTI == nullptr*/)
	{
		return ETeamAttitude::Neutral;
	}

	//Get Actor's TeamId
	FGenericTeamId OtherActorTeamId = NULL;
	if (BotTI != nullptr)
	{
		OtherActorTeamId = BotTI->GetGenericTeamId();
	}
	//else if (PlayerTI != nullptr)
	//{
	//	OtherActorTeamId = PlayerTI->GetGenericTeamId();
	//}

	//Check if Hostile
	FGenericTeamId ThisId = GetGenericTeamId(); // 이 컨트롤러를 빙의한 엑터의 TeamId
	if(OtherActorTeamId == 8) // OtherActorTeamId가 8이라면 (본인이 원하는 int값으로 변경하여 그 값을 경우 Neutral이 리턴되도록 코딩해도 됨)
	{
		return ETeamAttitude::Neutral; // 중립
	}
	else if (OtherActorTeamId == ThisId)
	{
		return ETeamAttitude::Friendly; // 프렌드
	}
	else
	{
		return ETeamAttitude::Hostile; // 적
	}
}

void ACAIController::OnPerception(AActor* Actor, FAIStimulus Stimulus)
{	
	if (IsPlayerOnly && Actor && Actor->ActorHasTag("PlayerFriend"))
	{
		IsPlayerOnly = false;
		Blackboard->SetValueAsObject("Opponent", NULL);
		Enemy->SetOpponent(false, NULL);
	}
	
	if (Enemy && Stimulus.WasSuccessfullySensed() && !IsPlayerOnly)
	{
		TArray<AActor*>Threats;
		PerceptionComponent->GetHostileActors(Threats);

		if (Threats.Num() <= 0)
		{
			return;
		}

		const int32 i = Threats.Find(Actor);

		if (i < 0)
		{
			return;
		}
		
		if (Actor)
		{
			
			// 블랙보드에 Opponent가 할당되어있으면 그냥 함수를 끝낸다
			if (Blackboard->GetValueAsObject("Opponent")) 
				return;
			// 블랙보드에 Opponent가 할당되어있지 않으면 
			else
			{
				
				int32 select = UKismetMathLibrary::RandomIntegerInRange(0, Threats.Num() - 1);
				// 랜덤으로 선택된 Hostile 중 Hostile로 인식된 상대가 공격받는 중인 대상이 4마리 이하이면 Opponent로 설정
				if (Cast<ACEnemy>(Threats[select])->GetTargettedActorCount() < 5)
				{	
					Blackboard->SetValueAsObject("Opponent", Cast<ACharacter>(Threats[select]));
					Enemy->SetOpponent(Stimulus.WasSuccessfullySensed(), Cast<ACharacter>(Threats[select]));
					//SetFocus(Stimulus.WasSuccessfullySensed() ? Cast<ACharacter>((Threats[select])) : nullptr);

					return;
				}
				// 4마리 초과일 경우 Player로 Target 설정
				else
				{
					Blackboard->SetValueAsObject("Opponent", Cast<ACharacter>(Player));
					Enemy->SetOpponent(Stimulus.WasSuccessfullySensed(), Cast<ACharacter>(Player));
				}
			}
		}
	}

	if (IsPlayerOnly)
	{
		TArray<AActor*>outPlayerArr;

		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPlayer::StaticClass(), outPlayerArr);

		for (int i = 0; i < outPlayerArr.Num(); i++)
		{
			if (outPlayerArr[i])
				Player = Cast<ACPlayer>(outPlayerArr[i]);
		}
		
		if (Player && Enemy)
		{
			Blackboard->SetValueAsObject("Opponent", Player);
			Enemy->SetOpponent(true, Player);
		}
	}
	
	
	
	//IGenericTeamAgentInterface* teamInterface = Cast<IGenericTeamAgentInterface>(Actor);

	// Sight에 인식된 Actor들을 outActorArr에 저장
	//TArray<AActor*> outActorArr;
	//Perception->GetPerceivedActors(UAISenseConfig_Sight::StaticClass(), outActorArr);

	//for (int i = 0; i < outActorArr.Num(); i++)
	//{
	//	IGenericTeamAgentInterface* teamInterface = Cast<IGenericTeamAgentInterface>(outActorArr[i]);

	//	if (teamInterface)
	//	{
	//		if (GetGenericTeamId() == teamInterface->GetGenericTeamId())
	//		{
	//			
	//	
	//			return;
	//		}
	//		else if(GetGenericTeamId() != teamInterface->GetGenericTeamId())
	//		{
	//			Enemy->SetOpponent(Cast<ACharacter>(teamInterface));
	//			Blackboard->SetValueAsObject("Opponent", Cast<ACharacter>(teamInterface));
	//		}
	//	}
	//}

	//if (enemy == nullptr)
	//{
	//	return;
	//}
	//

	//Enemy->SetTarget(Stimulus.WasSuccessfullySensed(), Actor);
	//SetFocus(Stimulus.WasSuccessfullySensed() ? enemy : nullptr);
}