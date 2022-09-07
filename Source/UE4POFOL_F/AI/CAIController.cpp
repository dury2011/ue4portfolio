/* MEMO: CAIController.cpp �ڵ� ��ġ ���� �Ϸ� */

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

	//Sight ����
	Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight");

	//Perception ����
	AIPerceptionComponent->ConfigureSense(*Sight);
	AIPerceptionComponent->SetDominantSense(*Sight->GetSenseImplementation());
}

void ACAIController::BeginPlay()
{
	Super::BeginPlay();

	// ��Ʈ������ ������ �ڵ�
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
	// Controller����Actor���ų�, ��밡 ������ ����
	if (Enemy == nullptr) return;
	if (Enemy->GetOpponent()) return;
	if (IsBossController) return;

	// Threats�� ���Ұ� ���ٸ� ����
	PerceptionComponent->GetHostileActors(Threats);

	if (Threats.Num() <= 0) return;

	// ���� ��찡 �ƴϸ� Perception�� ������ Hostile�� ã�Ƽ� ���� ��������
	int32 select = UKismetMathLibrary::RandomIntegerInRange(0, Threats.Num() - 1);

	// OnPerception���� Hostile���� Threats�� �Ҵ��ߴ�. ���� ���� ���� ���õ� Threats�� �׾ ���� ���¶�� ���� �� �ֱ� ������ ���� 
	// �����ص� SetTimer�� ���Ͽ� �ٽ� ��
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
 * �Ʒ� �Լ����� ���ϰ��� ��� ��� ���Ǵ°ǰ�? */

ETeamAttitude::Type ACAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (!InMission2)
	{
		//�þ߿� ���� ���Ͱ� �� �̻����� Ȯ��
		const APawn* OtherPawn = Cast<APawn>(&Other);
		if(OtherPawn == nullptr)
			return ETeamAttitude::Neutral;

		//���Ͱ� IGenericTeamAgentInterface�� ��ӹ޾Ҵ°� Ȯ��
		// Player�� AIController�� �����Ƿ� 
		//auto PlayerTI = Cast <IGenericTeamAgentInterface>(&Other);

		// Bot(Enemy)�� AIContorller�� �����Ƿ� 
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
		FGenericTeamId ThisId = GetGenericTeamId(); // �� ��Ʈ�ѷ��� ������ ������ TeamId
		if(OtherActorTeamId == 8) // OtherActorTeamId�� 8�̶�� (������ ���ϴ� int������ �����Ͽ� �� ���� ��� Neutral�� ���ϵǵ��� �ڵ��ص� ��)
			return ETeamAttitude::Neutral; // �߸�
		else if (OtherActorTeamId == ThisId)
			return ETeamAttitude::Friendly; // ������
		else
			return ETeamAttitude::Hostile; // ��
	}
	else 
		return ETeamAttitude::Neutral;
}

void ACAIController::OnPerception(AActor* Actor, FAIStimulus Stimulus)
{	
	//// TeamId�� ���Ͽ� Hostile�� Actor�� �߷��� ���� �迭Threats�� �Ҵ��Ͽ� Actor�� Hostile�� �ƴϸ� ����
	//// ������ Hostile Actor�� PlayerFriend Tag�� ������ IsPlayerOnly = false
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
	//// Controller�� IsPlayerOnly�� ������, Perception�� ������ Actor�� Tag�� PlayerFriend�� ���
	//if (IsPlayerOnly && Actor && Actor->ActorHasTag("PlayerFriend"))
	//{
	//	IsPlayerOnly = false;
	//	Blackboard->SetValueAsObject("Opponent", NULL);
	//	Enemy->SetOpponent(false, NULL);
	//}
	//if (!IsPlayerOnly && Enemy && Stimulus.WasSuccessfullySensed() && Actor)
	//{
	//	// �����忡 Opponent�� �Ҵ�Ǿ������� �׳� �Լ��� ������
	//	if (Blackboard->GetValueAsObject("Opponent")) 
	//		return;
	//	// �����忡 Opponent�� �Ҵ�Ǿ����� ������ 
	//	else
	//	{
	//
	//
	//		int32 select = UKismetMathLibrary::RandomIntegerInRange(0, Threats.Num() - 1);
	//		// �������� ���õ� Hostile �� Hostile�� �νĵ� ��밡 ���ݹ޴� ���� ����� 4���� �����̸� Opponent�� ����
	//		if (Cast<ACEnemy>(Threats[select])->GetTargettedActorCount() < 5)
	//		{	
	//			Blackboard->SetValueAsObject("Opponent", Cast<ACharacter>(Threats[select]));
	//			Enemy->SetOpponent(Stimulus.WasSuccessfullySensed(), Cast<ACharacter>(Threats[select]));
	//			//SetFocus(Stimulus.WasSuccessfullySensed() ? Cast<ACharacter>((Threats[select])) : nullptr);
	//
	//			return;
	//		}
	//		// ~ 4���� �ʰ��� ��� Player�� Target ���� �ٵ� �̷����ϸ� PlayerFriend�� ���Player�� Opponent�� �� �� �ִ�.
	//		else if()
	//		{
	//			Blackboard->SetValueAsObject("Opponent", Cast<ACharacter>(Player));
	//			Enemy->SetOpponent(Stimulus.WasSuccessfullySensed(), Cast<ACharacter>(Player));
	//		}
	//	}
	//}
	//IGenericTeamAgentInterface* teamInterface = Cast<IGenericTeamAgentInterface>(Actor);
	//
	// Sight�� �νĵ� Actor���� outActorArr�� ����
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
	// �� �Լ��� Perception�� �� ������ ȣ��ȴ�. ���⼭�� Sight�� Perception�� ������ ȣ��ȴ�. 
    // �Ķ���� Actor�� Perception�� Actor�̰�, �Ķ���� Stimulus�� Perception�� ���õ� �����̴�.
	//
	// �켱 �Ķ���� Actor�� Hostile���� �ƴ��� Ȯ���ؾ��Ѵ�.
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