// MEMO: CCharacterComponent.cpp 코드 배치 정리 완료
#include "Component/CCharacterComponent.h"
#include "Global.h"
#include "Weapon/CWeaponStructure.h"
#include "GameFramework/Character.h"
#include "Weapon/CWeapon.h"
#include "Weapon/CProjectile.h"
#include "Effect/CGhostTrail.h"

UCCharacterComponent::UCCharacterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCCharacterComponent::BeginPlay()
{
	Super::BeginPlay();

	Hp = MaxHp;
	Mp = MaxMp;
	Sp = MaxSp;
	
	OwnerCharacter = Cast<ACharacter>(GetOwner());

	if (OwnerCharacter)
	{
		FActorSpawnParameters params;
		params.Owner = Cast<AActor>(OwnerCharacter);

		for (int i = 0; i < (int32)EWeaponType::Max; i++)
		{
			if (NormalWeaponClasses[i])
				NormalWeapon[i] = OwnerCharacter->GetWorld()->SpawnActor<ACWeapon>(NormalWeaponClasses[i], params);
		}


		OwnerCharacter->GetWorldTimerManager().SetTimer(UpdateSpTimer, this, &UCCharacterComponent::UpdateSp, 1.0f, true, 2.0f);
		OwnerCharacter->GetWorldTimerManager().SetTimer(UpdateMpTimer, this, &UCCharacterComponent::UpdateMp, 1.0f, true, 1.0f);
	}
}

void UCCharacterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCCharacterComponent::SetHp(float InHp)
{
	float check = Hp;
	check += InHp;

	if (check >= MaxHp)
		return;

	Hp += InHp;
}

void UCCharacterComponent::SetMp(float InMp)
{
	float check = Mp;
	check += InMp;
	
	if (check >= MaxMp)
		return;
	
	Mp += InMp;
}

void UCCharacterComponent::SetSp(float InSp)
{
	float check = Sp;
	check += InSp;
	
	if (check >= MaxSp)
	{
		Sp = MaxSp;

		return;
	}
	
	Sp += InSp;
}

void UCCharacterComponent::SetCurrentStateType(EStateType InType)
{
	ChangeStateType(InType);
}

void UCCharacterComponent::SetCurrentWeaponType(EWeaponType InType)
{
	ChangeWeaponType(InType);
}

void UCCharacterComponent::ChangeStateType(EStateType InType)
{
	PreviousStateType = CurrentStateType;
	CurrentStateType = InType;

	if (OnStateTypeChange.IsBound())
		OnStateTypeChange.Broadcast(CurrentStateType, PreviousStateType);
}

void UCCharacterComponent::ChangeWeaponType(EWeaponType InType)
{
	PreviousWeaponType = CurrentWeaponType;
	CurrentWeaponType = InType;

	if (OnWeaponTypeChange.IsBound())
		OnWeaponTypeChange.Broadcast(CurrentWeaponType, PreviousWeaponType);
}

void UCCharacterComponent::UpdateSp()
{
	Sp += 300;

	if (Sp > MaxSp)
		Sp = MaxSp;
}

void UCCharacterComponent::UpdateMp()
{
	Mp += 300;

	if (Mp > MaxMp)
		Mp = MaxMp;
}

