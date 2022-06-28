#include "Enemy/CEnemy_Buff_Red.h"
#include "Global.h"
#include "Components/CapsuleComponent.h"
#include "Component/CCharacterComponent.h"

ACEnemy_Buff_Red::ACEnemy_Buff_Red()
{
	USkeletalMesh* skeletalMesh;
	ConstructorHelpers::FObjectFinder<USkeletalMesh> asset(*FString("SkeletalMesh'/Game/ParagonMinions/Characters/Buff/Buff_Red/Meshes/Buff_Red.Buff_Red'"));

	if (asset.Succeeded())
	{
		skeletalMesh = asset.Object;
		GetMesh()->SetSkeletalMesh(skeletalMesh);
		GetMesh()->SetupAttachment(GetCapsuleComponent());
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}

	//ConstructorHelpers::FClassFinder<UAnimInstance> animInstance(*FString("AnimBlueprint'/Game/FORUE4POFOL/Enemy/Enemy_Buff_Red/Blueprint/ABP_Enemy_Buff_Red.ABP_Enemy_Buff_Red_C'"));

	//if (animInstance.Succeeded())
	//{
	//	AnimInstance = animInstance.Class;
	//	GetMesh()->SetAnimInstanceClass(AnimInstance);
	//}

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	bActivateRotateToOpponent = false;
}

void ACEnemy_Buff_Red::BeginPlay()
{
	Super::BeginPlay();
}

void ACEnemy_Buff_Red::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACEnemy_Buff_Red::OnAttack()
{
	bActivateRotateToOpponent = true;
	
	CharacterComponent->SetIsMontagePlaying(true);

	int select = UKismetMathLibrary::RandomInteger(2);

	switch (select)
	{
		case 0:
		{
			CharacterComponent->GetActionDatasOnehand(0).PlayMontage(this);

			break;
		}
		case 1:
		{
			CharacterComponent->GetActionDatasOnehand(1).PlayMontage(this);

			break;
		}
		case 2:
		{
			CharacterComponent->GetActionDatasOnehand(2).PlayMontage(this);

			break;
		}
		default:
			break;
	}
}
