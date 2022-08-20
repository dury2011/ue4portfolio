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
#include "Interface/CInterface_Interaction.h"

ACAIController::ACAIController(const FObjectInitializer& ObjectInitializer)
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("Perception");

	//Sight 설정
	Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight");

	//Perception 설정
	AIPerceptionComponent->ConfigureSense(*Sight);
	AIPerceptionComponent->SetDominantSense(*Sight->GetSenseImplementation());

	//ACEnemy* enemy = Cast<ACEnemy>(GetOwner());

	//if (enemy)
	//{
	//	Enemy = enemy;
	//	TeamId = FGenericTeamId(Enemy->ID);

	//	Blackboard->SetValueAsInt("ThisTeamID", Enemy->ID);

	//	TArray<AActor*>outPlayerArr;

	//	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Player", outPlayerArr);

	//	for (int i = 0; i < outPlayerArr.Num(); i++)
	//	{
	//		if (outPlayerArr[i])
	//			Player = Cast<ACPlayer>(outPlayerArr[i]);
	//	}

	//	if (Player)
	//		Blackboard->SetValueAsInt("PlayerTeamId", Player->ID);
	//}

}

void ACAIController::BeginPlay()
{
	Super::BeginPlay();

	// 포트폴리오 디버깅용 코드
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
}

void ACAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (Enemy)
	{
		Blackboard->SetValueAsEnum("State", (uint8)Enemy->GetCurrentEnemyStateType());
		Blackboard->SetValueAsObject("Opponent", Enemy->GetOpponent());

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

			//if ( (Enemy->GetCurrentEnemyStateType() == EEnemyStateType::Damage) && !IsDamaged)
			//{
			//	IsDamaged = true;
			//	
			//	BrainComponent->StopLogic(FString("Enemy Skill Damage"));
			//}
			//else if ((Enemy->GetCurrentEnemyStateType() != EEnemyStateType::Damage) && IsDamaged)
			//{
			//	IsDamaged = false;
			//	
			//	BrainComponent->RestartLogic();
			//}
		}
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

	if (Enemy && AIPerceptionComponent)
	{
		Enemy->OnEnemyDiedStopAI.AddDynamic(this, &ACAIController::StopEnemyAI);
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ACAIController::OnPerception);

		TeamId = FGenericTeamId(Enemy->ID);

		Blackboard->SetValueAsInt("ThisTeamID", Enemy->ID);

		TArray<AActor*>outPlayerArr;

		UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Player", outPlayerArr);

		for (int i = 0; i < outPlayerArr.Num(); i++)
		{
			if (outPlayerArr[i])
				Player = Cast<ACPlayer>(outPlayerArr[i]);
		}

		if (Player)
			Blackboard->SetValueAsInt("PlayerTeamId", Player->ID);
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
		return ETeamAttitude::Neutral;

	//엑터가 IGenericTeamAgentInterface를 상속받았는가 확인
	// Player는 AIController가 없으므로 
	auto PlayerTI = Cast <IGenericTeamAgentInterface>(&Other);

	// Bot(Enemy)는 AIContorller가 있으므로 
	class IGenericTeamAgentInterface* BotTI = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController());
	
	if(BotTI == nullptr && PlayerTI == nullptr)
		return ETeamAttitude::Neutral;

	//Get Actor's TeamId
	FGenericTeamId OtherActorTeamId = NULL;
	if (BotTI != nullptr)
		OtherActorTeamId = BotTI->GetGenericTeamId();
	else if (PlayerTI != nullptr)
		OtherActorTeamId = PlayerTI->GetGenericTeamId();

	//Check if Hostile
	FGenericTeamId ThisId = GetGenericTeamId(); // 이 컨트롤러를 빙의한 엑터의 TeamId
	if(OtherActorTeamId == 8) // OtherActorTeamId가 8이라면 (본인이 원하는 int값으로 변경하여 그 값을 경우 Neutral이 리턴되도록 코딩해도 됨)
		return ETeamAttitude::Neutral; // 중립
	else if (OtherActorTeamId == ThisId)
		return ETeamAttitude::Friendly; // 프렌드
	else
		return ETeamAttitude::Hostile; // 적
}

void ACAIController::OnPerception(AActor* Actor, FAIStimulus Stimulus)
{	
	//// TeamId를 통하여 Hostile인 Actor만 추려낸 것을 배열Threats에 할당하여 Actor가 Hostile이 아니면 리턴
	//// 감지된 Hostile Actor가 PlayerFriend Tag를 가지면 IsPlayerOnly = false
	//TArray<AActor*>threats;
	//
	//AIPerceptionComponent->GetHostileActors(threats);
	//
	//const int32 i = threats.Find(Actor);
	//
	//if (threats.Num() <= 0) return;
	//
	//if (i < 0) return;
	//
	//if (Enemy->ActorHasTag("Enemy") && IsPlayerOnly)
	//{
	//	IsPlayerOnly = false;
	//	Blackboard->SetValueAsObject("Opponent", NULL);
	//	Enemy->SetOpponent(false, NULL);
	//}
	//
	//
	////if (Blackboard->GetValueAsObject("Opponent")) return;
	//else if(HostileCount < 4)
	//{
	//	int32 select = UKismetMathLibrary::RandomIntegerInRange(0, threats.Num() - 1);
	//	
	//	Blackboard->SetValueAsObject("Opponent", threats[select]);
	//	Enemy->SetOpponent(Stimulus.WasSuccessfullySensed(), Cast<ACharacter>(threats[select]));
	//	HostileCount++;
	//}
	//else if (Enemy->ActorHasTag("Enemy"))
	//{
	//	Blackboard->SetValueAsObject("Opponent", Player);
	//	Enemy->SetOpponent(Stimulus.WasSuccessfullySensed(), Cast<ACharacter>(Player));
	//}
	//else if (Enemy->ActorHasTag("PlayerFriend"))
	//{	
	//	int32 selectNew = UKismetMathLibrary::RandomIntegerInRange(0, threats.Num() - 1);
	//	
	//	Blackboard->SetValueAsObject("Opponent", threats[selectNew]);
	//	Enemy->SetOpponent(Stimulus.WasSuccessfullySensed(), Cast<ACharacter>(threats[selectNew]));
	//}
	//// Controller가 IsPlayerOnly만 따르고, Perception에 감지된 Actor의 Tag가 PlayerFriend일 경우
	//if (IsPlayerOnly && Actor && Actor->ActorHasTag("PlayerFriend"))
	//{
	//	IsPlayerOnly = false;
	//	Blackboard->SetValueAsObject("Opponent", NULL);
	//	Enemy->SetOpponent(false, NULL);
	//}
	//if (!IsPlayerOnly && Enemy && Stimulus.WasSuccessfullySensed() && Actor)
	//{
	//	// 블랙보드에 Opponent가 할당되어있으면 그냥 함수를 끝낸다
	//	if (Blackboard->GetValueAsObject("Opponent")) 
	//		return;
	//	// 블랙보드에 Opponent가 할당되어있지 않으면 
	//	else
	//	{
	//
	//
	//		int32 select = UKismetMathLibrary::RandomIntegerInRange(0, Threats.Num() - 1);
	//		// 랜덤으로 선택된 Hostile 중 Hostile로 인식된 상대가 공격받는 중인 대상이 4마리 이하이면 Opponent로 설정
	//		if (Cast<ACEnemy>(Threats[select])->GetTargettedActorCount() < 5)
	//		{	
	//			Blackboard->SetValueAsObject("Opponent", Cast<ACharacter>(Threats[select]));
	//			Enemy->SetOpponent(Stimulus.WasSuccessfullySensed(), Cast<ACharacter>(Threats[select]));
	//			//SetFocus(Stimulus.WasSuccessfullySensed() ? Cast<ACharacter>((Threats[select])) : nullptr);
	//
	//			return;
	//		}
	//		// ~ 4마리 초과일 경우 Player로 Target 설정 근데 이렇게하면 PlayerFriend의 경우Player가 Opponent가 될 수 있다.
	//		else if()
	//		{
	//			Blackboard->SetValueAsObject("Opponent", Cast<ACharacter>(Player));
	//			Enemy->SetOpponent(Stimulus.WasSuccessfullySensed(), Cast<ACharacter>(Player));
	//		}
	//	}
	//}
	//IGenericTeamAgentInterface* teamInterface = Cast<IGenericTeamAgentInterface>(Actor);
	//
	// Sight에 인식된 Actor들을 outActorArr에 저장
	//TArray<AActor*> outActorArr;
	//Perception->GetPerceivedActors(UAISenseConfig_Sight::StaticClass(), outActorArr);
	//
	//for (int i = 0; i < outActorArr.Num(); i++)
	//{
	//	IGenericTeamAgentInterface* teamInterface = Cast<IGenericTeamAgentInterface>(outActorArr[i]);
	//
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
	//
	//if (enemy == nullptr)
	//{
	//	return;
	//}
	//
	//
	//Enemy->SetTarget(Stimulus.WasSuccessfullySensed(), Actor);
	//SetFocus(Stimulus.WasSuccessfullySensed() ? enemy : nullptr);

	// 이 함수는 Perception이 될 때마다 호출된다. 여기서는 Sight에 Perception될 때마다 호출된다. 
    // 파라미터 Actor는 Perception된 Actor이고, 파라미터 Stimulus는 Perception과 관련된 정보이다.

	// 우선 파라미터 Actor가 Hostile인지 아닌지 확인해야한다.
	if (Enemy)
	{
		TArray<AActor*>threats;
		/*AI*/PerceptionComponent->GetHostileActors(threats);
		const int32 i = threats.Find(Actor);
		if (threats.Num() <= 0) return;
		if (i < 0) return;

		if(!Enemy->GetOpponent() && Stimulus.WasSuccessfullySensed())
		{
			int32 select = UKismetMathLibrary::RandomIntegerInRange(0, threats.Num() - 1);
			ICInterface_Interaction* threatInterface = Cast<ICInterface_Interaction>(threats[select]);

			if (threatInterface)
				threatInterface->IncrementTargettedCount();

			if (threatInterface->GetTargettedCount() < 4)
				Enemy->SetOpponent(Stimulus.WasSuccessfullySensed(), Cast<ACharacter>(threats[select]));
			
		}
	}
}