#include "Notify/CAnimNotify_Launch.h"
#include "Global.h"
#include "Player/CPlayer.h"
#include "Kismet/KismetSystemLibrary.h"
//#include "Component/CStateComponent.h"
#include "Containers/EnumAsByte.h"
#include "Enemy/CEnemy.h"

FString UCAnimNotify_Launch::GetNotifyName_Implementation() const
{
	return "Launch";
}

void UCAnimNotify_Launch::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	//Super::Notify(MeshComp, Animation);
	//
	//CheckNull(MeshComp);
	////
	////UCStateComponent* stateComponent = CHelpers::GetComponent<UCStateComponent>(MeshComp->GetOwner());
	//
	////ACharacter* character = Cast<ACPlayer>(MeshComp->GetOwner());
	//
	////if (!character && !stateComponent)
	////{
	//
	//ACEnemy* enemy = Cast<ACEnemy>(MeshComp->GetOwner());
	//ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());
	//
	//if (boss)
	//{
	//	FVector forwardVector = boss->GetActorForwardVector();
	//	
	//	if (boss->BossComponent->IsAttackShieldMode())
	//	{
	//		boss->LaunchCharacter(forwardVector * 12000.0f, false, false);
	//		
	//		GLog->Log("Launch Boss (AttackShield)");
	//	}
	//	else
	//	{
	//		boss->LaunchCharacter(forwardVector * 5000.0f, false, false);
	//
	//		GLog->Log("Launch Boss (Normal)");
	//	}
	//
	//	return;
	//}
	//
	//if (enemy)
	//{
	//	FVector forwardVector = enemy->GetActorForwardVector();
	//
	//	enemy->LaunchCharacter(forwardVector * 300.0f, false, false);
	//
	//	GLog->Log("Launch Enemy");
	//
	//	return;
	//}
	//
	//if (player)
	//{
	//	FVector forwardVector = player->GetActorForwardVector();
	//
	//	player->LaunchCharacter(forwardVector * 3000.0f, false, false);
	//
	//	GLog->Log("Launch Player");
	//
	//	return;
	//}
	//
	////	return;
	////}
	//
	////if (stateComponent->IsAttackSkillMode())
	////{
	////	FVector forwardVector = character->GetActorForwardVector();
	////	character->LaunchCharacter(FVector(forwardVector.X * 3000.0f, forwardVector.Y * 3000.0f, 0.0f), false, false);
	////}
	////else if (stateComponent->IsAttackNormalMode())
	////{
	////	FVector forwardVector = character->GetActorForwardVector();
	////	character->LaunchCharacter(FVector(forwardVector.X * 500.0f, forwardVector.Y * 500.0f, 0.0f), false, false);
	////}
	//

	CheckNull(MeshComp);

	ACharacter* character = Cast<ACharacter>(MeshComp->GetOwner());

	if (character)
	{
		FVector direction = FVector::ZeroVector;
		
		switch(LaunchDirectionType)
		{
		case ECharacterLaunchDirectionType::Foward :
			direction = character->GetActorForwardVector();
			break;

		case ECharacterLaunchDirectionType::Backward :
			direction = - character->GetActorForwardVector();
			break;

		case ECharacterLaunchDirectionType::Left:
			direction = character->GetActorRightVector();
			break;

		case ECharacterLaunchDirectionType::Right:
			direction = - character->GetActorRightVector();
			break;

		default:
			break;
		}

		character->LaunchCharacter(direction * LaunchLength, true, true);
	}
}