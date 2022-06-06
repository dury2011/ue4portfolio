/* MEMO: CAnimNotify_BossLandingPos.cpp �ڵ� ��ġ ���� �Ϸ� */
#include "Notify/CAnimNotify_BossLandingPos.h"
#include "Global.h"
#include "Boss/CBoss.h"
#include "Components/CapsuleComponent.h"

FString UCAnimNotify_BossLandingPos::GetNotifyName_Implementation() const
{
	return FString("BossLandPos");
}

void UCAnimNotify_BossLandingPos::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	
	CheckNull(MeshComp);

	Boss = Cast<ACBoss>(MeshComp->GetOwner());

	if (Boss)
		if (Boss->GetOpponent())
			Boss->MoveOpponentLoc();	
}
