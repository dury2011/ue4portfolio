/* MEMO: CBTService_Enemy.cpp 코드 배치 정리 완료 */
#include "AI/CBTService_Enemy.h"
#include "Global.h"
#include "Player/CPlayer.h"
#include "Enemy/CEnemy.h"
#include "AI/CAIController.h"
// TODO: 바로 아래 해더 곧 삭제 
#include "GameFramework/Pawn.h"
#include "GameFramework/Character.h"

#include "DrawDebugHelpers.h"
#include "Component/CCharacterComponent.h"

//#define LOG_CBTSERVICE_ENEMY

UCBTService_Enemy::UCBTService_Enemy()
{
	NodeName = "Enemy";

	Interval = 0.1f;
	RandomDeviation = 0.0f;
}

void UCBTService_Enemy::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ACAIController* controller = Cast<ACAIController>(OwnerComp.GetOwner());
	ACEnemy* enemy = Cast<ACEnemy>(controller->GetPawn());
	ACharacter* opponent = enemy->GetOpponent();

	float distance = enemy->GetDistanceTo(opponent);
	//UCStateComponent* stateComponent = CHelpers::GetComponent<UCStateComponent>(enemy);
	//UCAIStateComponent* aIStateComponent = CHelpers::GetComponent<UCAIStateComponent>(enemy);
	//UCWeaponComponent* weaponComponent = CHelpers::GetComponent<UCWeaponComponent>(enemy);
	//ACPlayer* player = aIStateComponent->GetTargetPlayer();
	//float distance = enemy->GetDistanceTo(player);

	if (opponent)
	{
		if (distance <= 450.0f)
			enemy->OnStateTypeChange(EEnemyStateType::Attack);
		else
			enemy->OnStateTypeChange(EEnemyStateType::Walk);
	}

	//if (player)
	//{
	//
	//}
	//
	//if (player == nullptr) 
	//{
	//
	//}
	//else
	//{
		//if (distance >= 900.f)
		//{
		//	aIStateComponent->SetAttackRifleMode();
	//
		//	return;
		//}
		//
		//{
			//if (distance <= 400.f)
			//{
				//aIStateComponent->SetAttackOnehandMode();
	//
				//return;
			//}
			//else
			//{
				//aIStateComponent->SetApproachMode();
		//	
			//	return;
			//}
		//}	
	//}
	
#ifdef LOG_CBTSERVICE_ENEMY
	// patrol & attack rifle radius
	DrawDebugSphere(GetWorld(), enemy->GetActorLocation(), 1500.f, 12, FColor::Blue, false);
	DrawDebugSphere(GetWorld(), enemy->GetActorLocation(), 500.f, 12, FColor::Red, false);
#endif LOG_CBTSERVICE_ENEMY
}
