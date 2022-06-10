/* MEMO: CAnimInstance_Enemy.cpp 코드 배치 정리 완료 */
#include "Animation/CAnimInstance_Enemy.h"
#include "Global.h"
#include "Enemy/CEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"


void UCAnimInstance_Enemy::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	Enemy = Cast<ACEnemy>(TryGetPawnOwner());
	
	CheckNull(Enemy);

	Enemy->OnEnemyStateTypeChanged.AddDynamic(this, &UCAnimInstance_Enemy::OnEnemyStateTypeChanged);
}

void UCAnimInstance_Enemy::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	CheckNull(Enemy);
	
	Speed = Enemy->GetVelocity().Size2D();
	Direction = CalculateDirection(Enemy->GetVelocity(), Enemy->GetActorRotation());
	Pitch = Enemy->GetBaseAimRotation().Pitch;
	bFalling = Enemy->GetCharacterMovement()->IsFalling();
	
	//UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Enemy_BossFriend"), ActorsTagEnemyBossFriend);
	//for (AActor* actor : ActorsTagEnemyBossFriend)
	//{
	//	if (!actor)
	//	{
	//	}
	//}
}

void UCAnimInstance_Enemy::OnEnemyStateTypeChanged(EEnemyStateType InPreviousType, EEnemyStateType InCurrentType)
{
	PreviousEnemyStateType = InPreviousType;
	CurrentEnemyStateType = InCurrentType;
}
