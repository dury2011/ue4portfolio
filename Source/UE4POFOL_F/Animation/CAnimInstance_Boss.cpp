#include "Animation/CAnimInstance_Boss.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "Component/CBossComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCAnimInstance_Boss::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	
	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	CheckNull(OwnerCharacter);
	
	BossComponent = CHelpers::GetComponent<UCBossComponent>(OwnerCharacter);
	
	BossComponent->OnBossStateTypeChanged.AddDynamic(this, &UCAnimInstance_Boss::BossStateTypeChanged);
}

void UCAnimInstance_Boss::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	CheckNull(OwnerCharacter);
	
	Speed = OwnerCharacter->GetVelocity().Size2D();
	IsFalling = OwnerCharacter->GetCharacterMovement()->IsFalling();
}

void UCAnimInstance_Boss::BossStateTypeChanged(EBossStateType InCurrentType, EBossStateType InPrevType)
{
	CurrentBossStateType = InCurrentType;
	PreviousBossStateType = InPrevType;
}