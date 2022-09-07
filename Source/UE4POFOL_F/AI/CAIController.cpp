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

	if (IsBossController)
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

void ACAIController::CheckOpponent()
{
	// Controller소유Actor없거나, 상대가 있으면 리턴
	if (Enemy == nullptr) return;
	if (Enemy->GetOpponent()) return;
	if (IsBossController) return;

	// Threats에 원소가 없다면 리턴
	PerceptionComponent->GetHostileActors(Threats);

	if (Threats.Num() <= 0) return;

	// 위의 경우가 아니면 Perception에 감지된 Hostile을 찾아서 상대로 설정해줌
	int32 select = UKismetMathLibrary::RandomIntegerInRange(0, Threats.Num() - 1);

	// OnPerception에서 Hostile들을 Threats에 할당했다. 게임 진행 도중 선택된 Threats가 죽어서 없는 상태라면 터질 수 있기 때문에 리턴 
	// 리턴해도 SetTimer에 의하여 다시 콜
	if (nullptr == Threats[select]) return;
	
	if(Enemy)
		Enemy->SetOpponent(true, Cast<ACharacter>(Threats[select]));
}

//void ACAIController::CheckDestroy()
//{
	//if (!InMission3) return;
	
	//if (Threats.Num() <= 0)
	//	Enemy->DestroyEnemy();
//}

void ACAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (Enemy && !IsBossController)
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
		}
	}
	else if (Enemy && IsBossController)
	{
		if (Player)
		{
			Blackboard->SetValueAsObject("Opponent", Player);
			Enemy->SetOpponent(true, Player);
		}
					
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
	
	Enemy = Cast<ACEnemy>(GetPawn());
	Enemy->OnEnemyDiedStopAI.AddDynamic(this, &ACAIController::StopEnemyAI);

	if (BehaviorTree)
	{	
		if(UseBlackboard(BehaviorTree->BlackboardAsset, Blackboard))
			RunBehaviorTree(BehaviorTree);

		Blackboard->SetValueAsVector("SpawnLocation", InPawn->GetActorLocation());
	}

	if (Enemy && AIPerceptionComponent)
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ACAIController::OnPerception);

		GetWorldTimerManager().SetTimer(OpponentCheckTimer, this, &ACAIController::CheckOpponent, 0.1f, true, 1.0f);
		//GetWorldTimerManager().SetTimer(DestroyCheckTimer, this, &ACAIController::CheckDestroy, 1.0f, true, 10.0f);

		if (!InMission2)
		{
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
		else if (InMission2)
		{
			TArray<AActor*>outSpawnerArr;

			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACCannon::StaticClass(), outSpawnerArr);

			for (int i = 0; i < outSpawnerArr.Num(); i++)
				if (outSpawnerArr[i])
				{
					Blackboard->SetValueAsObject("Opponent", outSpawnerArr[i]);
					Enemy->SetOpponent(true, Cast<ACharacter>(outSpawnerArr[i]));
				}
		}
	}

	IsSkillActivate = false;
}

void ACAIController::OnUnPossess()
{
	Super::OnUnPossess();

	//GetWorld()->GetTimerManager().ClearTimer(Timer);
}

void ACAIController::StopEnemyAI()
{
	Cast<UBehaviorTreeComponent>(BrainComponent)->StopTree();
	SetActorTickEnabled(false);
}

/* https://www.youtube.com/watch?v=HwBdnviT2vs 
 * 아래 함수에서 리턴값은 어디서 어떻게 사용되는건가? */

ETeamAttitude::Type ACAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (!InMission2)
	{
		//시야에 들어온 엑터가 폰 이상인지 확인
		const APawn* OtherPawn = Cast<APawn>(&Other);
		if(OtherPawn == nullptr)
			return ETeamAttitude::Neutral;

		//엑터가 IGenericTeamAgentInterface를 상속받았는가 확인
		// Player는 AIController가 없으므로 
		//auto PlayerTI = Cast <IGenericTeamAgentInterface>(&Other);

		// Bot(Enemy)는 AIContorller가 있으므로 
		class IGenericTeamAgentInterface* BotTI = Cast<IGenericTeamAgentInterface>(OtherPawn->GetController());
		
		if(BotTI == nullptr /*&& PlayerTI == nullptr*/)
			return ETeamAttitude::Neutral;

		//Get Actor's TeamId
		FGenericTeamId OtherActorTeamId = NULL;
		if (BotTI != nullptr)
			OtherActorTeamId = BotTI->GetGenericTeamId();
		//else if (PlayerTI != nullptr)
			//OtherActorTeamId = PlayerTI->GetGenericTeamId();

		//Check if Hostile
		FGenericTeamId ThisId = GetGenericTeamId(); // 이 컨트롤러를 빙의한 엑터의 TeamId
		if(OtherActorTeamId == 8) // OtherActorTeamId가 8이라면 (본인이 원하는 int값으로 변경하여 그 값을 경우 Neutral이 리턴되도록 코딩해도 됨)
			return ETeamAttitude::Neutral; // 중립
		else if (OtherActorTeamId == ThisId)
			return ETeamAttitude::Friendly; // 프렌드
		else
			return ETeamAttitude::Hostile; // 적
	}
	else 
		return ETeamAttitude::Neutral;
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
	//
	// 이 함수는 Perception이 될 때마다 호출된다. 여기서는 Sight에 Perception될 때마다 호출된다. 
    // 파라미터 Actor는 Perception된 Actor이고, 파라미터 Stimulus는 Perception과 관련된 정보이다.
	//
	// 우선 파라미터 Actor가 Hostile인지 아닌지 확인해야한다.
	if (!InMission2)
	{
		if (Enemy)
		{			
			PerceptionComponent->GetHostileActors(Threats);
			//const int32 i = Threats.Find(Actor);

			//if (Threats.Num() <= 0) return;
			//if (i < 0) return;
			//
			//if(!Enemy->GetOpponent())
			//{
			//	int32 select = UKismetMathLibrary::RandomIntegerInRange(0, Threats.Num() - 1);
			//
			//	Enemy->SetOpponent(Stimulus.WasSuccessfullySensed(), Cast<ACharacter>(Threats[select]));
			//}
		}
	}
}