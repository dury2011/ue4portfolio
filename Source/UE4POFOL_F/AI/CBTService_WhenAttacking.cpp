#include "AI/CBTService_WhenAttacking.h"
#include "Global.h"
#include "AI/CAIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Player/CPlayer.h"
#include "Component/CCharacterComponent.h"
#include "Enemy/CEnemy.h"

UCBTService_WhenAttacking::UCBTService_WhenAttacking()
{
	NodeName = "When Attacking";

	Interval = 0.1f;
	RandomDeviation = 0.0f;
}

void UCBTService_WhenAttacking::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* controllingPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (controllingPawn == nullptr)
		return;

	if (controllingPawn == nullptr)
		return;

	UWorld* world = controllingPawn->GetWorld();
	FVector center = controllingPawn->GetActorLocation();

	if (world == nullptr) 
		return;

	TArray<FOverlapResult> overlapResults;
	FCollisionQueryParams collisionQueryParam(NAME_None, false, controllingPawn);
	
	bool bResult = world->OverlapMultiByChannel(
		overlapResults,
		center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(OpponentDetectRadius),
		collisionQueryParam
	);

	if (bResult)
	{
		for (auto const& overlapResult : overlapResults)
		{
			ACharacter* character = Cast<ACharacter>(overlapResult.GetActor());

			if (character && character->GetController()->IsPlayerController())
			{
				//OwnerComp.GetBlackboardComponent()->SetValueAsObject("Player", character);

				ACPlayer* player = Cast<ACPlayer>(character);
				UCCharacterComponent* characterComponent = player->CharacterComponent;

				OwnerComp.GetBlackboardComponent()->SetValueAsBool("OpponentAttacking", player->GetbAttacking());


				return;
				//if (characterComponent->GetCurrentWeaponType() == EWeaponType::Onehand)
				//{
				//	if (characterComponent->GetCurrentStateType() == EStateType::Attack)
				//	{
				//		int32 rand = UKismetMathLibrary::RandomIntegerInRange(0, 1);
				//		
				//		switch(rand)
				//		{
				//			case 0:
				//			{
				//				OwnerComp.GetBlackboardComponent()->SetValueAsEnum("State", (uint8)EEnemyStateType::Parkour);
				//
				//				break;
				//			}
				//			case 1:
				//			{
				//				OwnerComp.GetBlackboardComponent()->SetValueAsEnum("State", (uint8)EEnemyStateType::Attack);
				//
				//				break;
				//			}
				//		}				
				//		return;
				//	}
				//}
				//else if (player->CharacterComponent->GetCurrentWeaponType() == EWeaponType::Spell)
				//{
				//	if (characterComponent->GetCurrentStateType() == EStateType::Attack)
				//	{
				//		ACEnemy* enemy = Cast<ACEnemy>(controllingPawn);
				//
				//		//TODO: 추후 예정
				//	}
				//	return;
				//}
			}
		}
	}
	OwnerComp.GetBlackboardComponent()->SetValueAsBool("OpponentAttacking", false);
}