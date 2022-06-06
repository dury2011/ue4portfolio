/* MEMO: UCAnimNotify_DamageLaunchUp.cpp �ڵ� ��ġ ���� �Ϸ� */
#include "Notify/CAnimNotify_DamageLaucnhUp.h"
#include "Global.h"
#include "Enemy/CEnemy.h"


FString UCAnimNotify_DamageLaucnhUp::GetNotifyName_Implementation() const
{
	return "Damage Launch Up";
}

void UCAnimNotify_DamageLaucnhUp::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);

	//Super::Notify(MeshComp, Animation);

	ACEnemy* character = Cast<ACEnemy>(MeshComp->GetOwner());

	if (character)
	{
		character->LaunchCharacter(FVector(0.0f, 0.0f, 200.0f), false, false);

		GLog->Log("Launch Up");
	}
}