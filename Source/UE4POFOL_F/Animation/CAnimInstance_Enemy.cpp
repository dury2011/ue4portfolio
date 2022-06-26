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
	Direction = CalculateDirection(Enemy->GetVelocity(), Enemy->GetControlRotation());
	Pitch = (Enemy->GetOpponent()->GetActorLocation() - Enemy->GetActorLocation()).Rotation().Pitch;
	Yaw = (Enemy->GetOpponent()->GetActorLocation() - Enemy->GetActorLocation()).Rotation().Yaw;
	bFalling = Enemy->GetCharacterMovement()->IsFalling();
	bDamage = Enemy->GetbDamage();
	
	if (Yaw >= -61.0f && Yaw <= 61.0f)
		bCanRotate = false;

	if (bCanRotate)
	{
		if (Yaw < -60.0f)
		{
			FRotator currentEnemyRotation = Enemy->GetActorRotation();
			FRotator targetEnemyRotation = FRotator(currentEnemyRotation.Pitch, currentEnemyRotation.Yaw -90.0f, currentEnemyRotation.Roll);
			Enemy->SetActorRotation(FMath::RInterpTo(currentEnemyRotation, targetEnemyRotation, DeltaSeconds, 5.0f));

			bCanRotate = false;
		}
		else if (Yaw > 60.0f)
		{
			FRotator currentEnemyRotation = Enemy->GetActorRotation();
			FRotator targetEnemyRotation = FRotator(currentEnemyRotation.Pitch, currentEnemyRotation.Yaw + 90.0f, currentEnemyRotation.Roll);
			Enemy->SetActorRotation(FMath::RInterpTo(currentEnemyRotation, targetEnemyRotation, DeltaSeconds, 5.0f));

			bCanRotate = true;
		}
	}
	//Pitch = Enemy->GetBaseAimRotation().Pitch;
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
