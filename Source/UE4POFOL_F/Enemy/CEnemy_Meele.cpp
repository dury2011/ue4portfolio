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
	
	if (!IsPlayerFriendly)
	{
		int select = UKismetMathLibrary::RandomIntegerInRange(0, 2);

		ActionDatas[select].PlayMontage(this);
	}
	else if (IsPlayerFriendly)
	{
		// MEMO: 일반 공격 3개는 기본 공격과 동일, 스킬공격 2개 (넉백 공격, 공중 콤보 공격)
		int select = UKismetMathLibrary::RandomIntegerInRange(0, 4);

		if (FriendlyActionDatas[select].Montage)
			FriendlyActionDatas[select].PlayMontage(this);
	}
}

