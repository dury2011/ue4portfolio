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
		GetMesh()->SetAnimInstanceClass(AnimBlueprint);
	}

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
	
	int select = UKismetMathLibrary::RandomInteger(3);

	Super::OnAttack();
	
	switch (2)
	{
		case 0:
		{
			ActionDatas[0].PlayMontage(this);

			break;
		}
		case 1:
		{
			ActionDatas[1].PlayMontage(this);

			break;
		}
		case 2:
		{
			ActionDatas[2].PlayMontage(this);

			break;
		}
		default:
			break;
	}
}
