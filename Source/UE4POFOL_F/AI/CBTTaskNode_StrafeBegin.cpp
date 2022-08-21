#include "AI/CBTTaskNode_StrafeBegin.h"
#include "Global.h"
#include "Enemy/CEnemy.h"
#include "AI/CAIController.h"

UCBTTaskNode_StrafeBegin::UCBTTaskNode_StrafeBegin()
{
	bNotifyTick = true;

	NodeName = "Begin Strafe";
}

EBTNodeResult::Type UCBTTaskNode_StrafeBegin::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type result = Super::ExecuteTask(OwnerComp, NodeMemory);

	ACEnemy* enemy = Cast<ACEnemy>(OwnerComp.GetAIOwner()->GetPawn());

	if (enemy == nullptr)
		return EBTNodeResult::Failed;
	
	//Enemy->GetWorldTimerManager().SetTimer(EndTimer, this, &UCBTTaskNode_StrafeBegin::EndStrafing, StrafingTime, false, StrafingTime);
	//Enemy->GetWorldTimerManager().SetTimer(ChangeDirectionTimer, this, &UCBTTaskNode_StrafeBegin::ChangeStrafing, ChangeDirectionTime, false, ChangeDirectionTime);
	
	FTimerHandle forceFinishTimer;
	GetWorld()->GetTimerManager().SetTimer(forceFinishTimer, FTimerDelegate::CreateLambda([&]()
	{
		//Enemy->EndStrafing();
		bFinish = true;
		StrafeDirection = FVector::ZeroVector;

	}), StrafingTime + 0.3f, false, StrafingTime + 0.3f);

	GetWorld()->GetTimerManager().SetTimer(EndTimer, FTimerDelegate::CreateLambda([&]() 
	{
		//Enemy->EndStrafing();
		bFinish = true;
		StrafeDirection = FVector::ZeroVector;

	}), StrafingTime, false, StrafingTime);
	
	GetWorld()->GetTimerManager().SetTimer(ChangeDirectionTimer, FTimerDelegate::CreateLambda([&]() 
	{
		//Enemy->ChangeStrafing();
		
		typedef EEnemyStrafingType s;

		int32 select = UKismetMathLibrary::RandomIntegerInRange(1, 3);

		switch ((EEnemyStrafingType)select)
		{
			case s::Front:
			{
				CurrentStrafingType = s::Front;

				StrafeDirection = OwnerComp.GetAIOwner()->GetPawn()->GetActorForwardVector() * 1.0f;

				break;
			}
			case s::Back:
			{
				CurrentStrafingType = s::Back;

				StrafeDirection = OwnerComp.GetAIOwner()->GetPawn()->GetActorForwardVector() * -1.0f;

				break;
			}
			case s::Left:
			{
				CurrentStrafingType = s::Left;

				StrafeDirection = OwnerComp.GetAIOwner()->GetPawn()->GetActorRightVector() * -1.0f;

				break;
			}
			case s::Right:
			{
				CurrentStrafingType = s::Right;

				StrafeDirection = OwnerComp.GetAIOwner()->GetPawn()->GetActorRightVector() * 1.0f;

				break;
			}
			default:
			{
				StrafeDirection = FVector::ZeroVector;

				break;
			}
		}
	}), ChangeDirectionTime, false, ChangeDirectionTime);
		
	bFinish = false;


	return EBTNodeResult::InProgress;
}

void UCBTTaskNode_StrafeBegin::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);


	if (!bFinish)
	{
		OwnerComp.GetAIOwner()->GetPawn()->AddMovementInput(StrafeDirection);
	}

	if (bFinish)
	{
		//bFinish = false;

		//Enemy->GetWorldTimerManager().ClearTimer(ChangeDirectionTimer);
		//Enemy->GetWorldTimerManager().ClearTimer(EndTimer);

		//Enemy = nullptr;

		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

//void UCBTTaskNode_StrafeBegin::ChangeStrafing()
//{
	//Enemy->ChangeStrafing();
//}

//void UCBTTaskNode_StrafeBegin::EndStrafing()
//{
	//Enemy->EndStrafing();
	//
	//bFinish = true;
//}


