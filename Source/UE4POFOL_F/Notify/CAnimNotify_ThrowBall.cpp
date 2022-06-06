/* MEMO: CAnimNotify_ThrowBall.cpp 코드 배치 정리 완료 */
#include "Notify/CAnimNotify_ThrowBall.h"
#include "Global.h"
#include "Boss/CBoss.h"
#include "Weapon/CBall.h"

FString UCAnimNotify_ThrowBall::GetNotifyName_Implementation() const
{
	return FString("Throw Ball");
}

void UCAnimNotify_ThrowBall::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	
	Boss = Cast<ACBoss>(MeshComp->GetOwner());

	if (Boss)
		Boss->ShootBall();
}