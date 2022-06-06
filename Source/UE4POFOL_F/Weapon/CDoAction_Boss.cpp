/* MEMO: CDoAction_Boss.cpp �ڵ� ��ġ ���� �Ϸ� */
#include "Weapon/CDoAction_Boss.h"
#include "Global.h"
#include "Boss/CBoss.h"
#include "Component/CBossComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
void UCDoAction_Boss::DoAction()
{
	Super::DoAction();
	
	ACBoss* boss = Cast<ACBoss>(OwnerCharacter);
	UCBossComponent* bossComponent = CHelpers::GetComponent<UCBossComponent>(boss);

	if (bossComponent->GetHpPercentage() >= 50.0f)
	{
		int select = FMath::RandRange(1, 3);

		switch (select)
		{
		case 1:
		{
			bossComponent->SetAttackNormalMode();

			ActionDatas[0].PlayMontage(OwnerCharacter);

			GLog->Log("Boss AttackNormal");

			break;
		}
		case 2:
		{
			bossComponent->SetAttackShieldMode();

			ActionDatas[1].PlayMontage(OwnerCharacter);

			GLog->Log("Boss AttackSheild");

			break;
		}
		case 3:
		{
			bossComponent->SetAttackJumpMode();

			ActionDatas[2].PlayMontage(OwnerCharacter);

			boss->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);

			GLog->Log("Boss AttackJump");

			break;
		}
		default:
			break;
		}
	}
	else
	{
		int select = FMath::RandRange(1, 3);

		switch (select)
		{
		case 1:
		{
			bossComponent->SetAttackRocketMode();

			ActionDatas[3].PlayMontage(OwnerCharacter);

			GLog->Log("Boss AttackRocket");

			break;
		}
		case 2:
		{
			bossComponent->SetAttackBallEightMode();

			ActionDatas[4].PlayMontage(OwnerCharacter);

			GLog->Log("Boss AttackBallEight");

			break;
		}
		case 3:
		{
			bossComponent->SetAttackCallFriendMode();

			ActionDatas[5].PlayMontage(OwnerCharacter);

			GLog->Log("Boss AttackCallFriend");

			break;
		}
		default:
			break;
		}
	}
}


void UCDoAction_Boss::OnWeaponBeginOverlap(class ACharacter* InAttacker, class AActor* InAttackCauser, class ACharacter* InOtherCharacter)
{	
	CheckNull(InOtherCharacter);

	Super::OnWeaponBeginOverlap(InAttacker, InAttackCauser, InOtherCharacter);

	ACBoss* boss = Cast<ACBoss>(OwnerCharacter);
	UCBossComponent* bossComponent = CHelpers::GetComponent<UCBossComponent>(boss);
	
	if (bossComponent->IsAttackNormalMode())
	{
		//HitDatas[0].SendDamage(OwnerCharacter, InAttackCauser, InOtherCharacter);
		
		GLog->Log("Player Hit By AttackNormal");
	}
	else if (bossComponent->IsAttackShieldMode())
	{
		//HitDatas[1].SendDamage(OwnerCharacter, InAttackCauser, InOtherCharacter);

		GLog->Log("Player Hit By AttackShield");
	}
	else if (bossComponent->IsAttackJumpMode())
	{
		//HitDatas[2].SendDamage(OwnerCharacter, InAttackCauser, InOtherCharacter);

		GLog->Log("Player Hit By AttackJump");
	}
	else if (bossComponent->IsAttackCallFriendMode())
	{
		//HitDatas[4].SendDamage(OwnerCharacter, InAttackCauser, InOtherCharacter);

		// ������ Enemy�� ��� �����ϹǷ� �Ϸ� �ȵ��� 
		GLog->Log("Player Hit By AttackCallFriend");
	}
	else if (bossComponent->IsAttackBallEightMode())
	{
		GLog->Log("Player Hit By AttackBallEight");
	}
}

void UCDoAction_Boss::OnWeaponEndOverlap(class ACharacter* InAttacker, class AActor* InAttackCauser, class ACharacter* InOtherCharacter)
{
	CheckNull(InOtherCharacter);

	Super::OnWeaponEndOverlap(InAttacker, InAttackCauser, InOtherCharacter);

	ACBoss* boss = Cast<ACBoss>(InAttacker);
}