/* MEMO: CAnimInstance_Enemy.cpp 코드 배치 정리 완료 */
#include "Animation/CAnimInstance_Enemy.h"
#include "Global.h"
#include "Weapon/CRifle.h"
#include "Interface/IRifle.h"
#include "Enemy/CEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"


void UCAnimInstance_Enemy::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	CheckNull(OwnerCharacter);

	//WeaponComponent = CHelpers::GetComponent<UCWeaponComponent>(OwnerCharacter);
	//AIStateComponent = CHelpers::GetComponent<UCAIStateComponent>(OwnerCharacter);

	//if (!!AIStateComponent)
	//{
	//	AIStateComponent->OnAIStateTypeChanged.AddDynamic(this, &UCAnimInstance_Enemy::OnAIStateTypeChanged);
	//	//AIStateComponent->OnAIStateTypeOnehandChanged.AddDynamic(this, &UCAnimInstance_Enemy::OnAIStateTypeOnehandChanged);
	//	//AIStateComponent->OnAIStateTypeRifleChanged.AddDynamic(this, &UCAnimInstance_Enemy::OnAIStateTypeRifleChanged);
	//}
}

void UCAnimInstance_Enemy::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	CheckNull(OwnerCharacter);
	//UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy_BossFriend"), ActorsTagEnemyBossFriend);

	//AIStateTypeOnehand = AIStateComponent->GetEAIStateTypeOnehand();
	//AIStateTypeRifle = AIStateComponent->GetEAIStateTypeRifle();
	
	Speed = OwnerCharacter->GetVelocity().Size2D();
	Direction = CalculateDirection(OwnerCharacter->GetVelocity(), OwnerCharacter->GetActorRotation()); 
	Pitch = OwnerCharacter->GetBaseAimRotation().Pitch;
	bFalling = OwnerCharacter->GetCharacterMovement()->IsFalling();
	//AIStateType = AIStateComponent->GetType();
	//bRifleEquipped = WeaponComponent->GetRifleForAnimInstance()->GetEquipped();
	//bRifleAiming = WeaponComponent->GetRifleForAnimInstance()->GetAiming();
	
	//for (AActor* actor : ActorsTagEnemyBossFriend)
	//{
	//	if (!actor)
	//	{
	//	}
	//}
}

//void UCAnimInstance_Enemy::OnAIStateTypeChanged(EAIStateType InNewType)
//{
//	AIStateType = InNewType;
//}

//void UCAnimInstance_Enemy::OnAIStateTypeOnehandChanged(EAIStateType_Onehand InNewType)
//{
//	AIStateTypeOnehand = InNewType;
//}
//
//void UCAnimInstance_Enemy::OnAIStateTypeRifleChanged(EAIStateType_Rifle InNewType)
//{
//	AIStateTypeRifle = InNewType;
//}

