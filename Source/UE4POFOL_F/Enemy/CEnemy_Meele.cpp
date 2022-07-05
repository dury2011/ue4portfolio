#include "Enemy/CEnemy_Meele.h"
#include "Global.h"
#include "Components/CapsuleComponent.h"
#include "Component/CCharacterComponent.h"

ACEnemy_Meele::ACEnemy_Meele()
{
	USkeletalMesh* skeletalMesh;
	ConstructorHelpers::FObjectFinder<USkeletalMesh> asset(*FString("SkeletalMesh'/Game/ParagonMinions/Characters/Minions/White_Camp_Minion/Meshes/Minion_melee.Minion_melee'"));
	
	if (asset.Succeeded())
	{
		skeletalMesh = asset.Object;
		GetMesh()->SetSkeletalMesh(skeletalMesh);
		GetMesh()->SetupAttachment(GetCapsuleComponent());
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		GetMesh()->SetAnimInstanceClass(AnimBlueprint);
	}
}

void ACEnemy_Meele::BeginPlay()
{
	Super::BeginPlay();
}

void ACEnemy_Meele::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACEnemy_Meele::OnAttack()
{
	Super::OnAttack();
	
	ActionDatas[0].PlayMontage(this);
}

