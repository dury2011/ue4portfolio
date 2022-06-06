/* MEMO: CBossActorComponent.cpp 코드 배치 정리 완료 */
#include "Component/CBossComponent.h"
#include "Global.h"
#include "Boss/CDataAsset_Boss.h"
#include "Weapon/CDoAction.h"

UCBossComponent::UCBossComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCBossComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());

	if (DataAsset)
		DataAsset->BeginPlay(OwnerCharacter);
}

void UCBossComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCBossComponent::DoAction()
{
	DataAsset->GetDoAction()->DoAction();
}

void UCBossComponent::SetHp(float InHp)
{
	Hp = InHp;
}

void UCBossComponent::AddHp(float InHp)
{
	Hp += InHp;
}

void UCBossComponent::SubHp(float InHp)
{
	Hp -= InHp;

	if (Hp <= 0.0f)
		Hp = 0.0f;
}

float UCBossComponent::GetHpPercentage()
{
	return Hp / FullHp * 100.0f;
}

void UCBossComponent::SetIdleMode()
{
	ChangeType(EBossStateType::Idle);
}

void UCBossComponent::SetAttackNormalMode()
{
	ChangeType(EBossStateType::AttackNormal);
}

void UCBossComponent::SetAttackShieldMode()
{
	ChangeType(EBossStateType::AttackShield);
}

void UCBossComponent::SetAttackJumpMode()
{
	ChangeType(EBossStateType::AttackJump);
}

void UCBossComponent::SetAttackRocketMode()
{
	ChangeType(EBossStateType::AttackRocket);
}

void UCBossComponent::SetAttackBallEightMode()
{
	ChangeType(EBossStateType::AttackBallEight);
}

void UCBossComponent::SetAttackCallFriendMode()
{
	ChangeType(EBossStateType::AttackCallFriend);
}

void UCBossComponent::ChangeType(EBossStateType InChangeType)
{
	EBossStateType prveType = CurrentType;
	
	CurrentType = InChangeType;
	
	if (OnBossStateTypeChanged.IsBound())
		OnBossStateTypeChanged.Broadcast(CurrentType, prveType);
}

