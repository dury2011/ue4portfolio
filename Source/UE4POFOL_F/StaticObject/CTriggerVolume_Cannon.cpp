#include "StaticObject/CTriggerVolume_Cannon.h"
#include "Global.h"

ACTriggerVolume_Cannon::ACTriggerVolume_Cannon()
{
	/* TODO: GameState�� �޾ƿͼ� Ư�� ������ �����ϸ� SetbCanTrigger�� true�� Set���ְų�,
	 * �������̽� Ŭ������ ���� Player�� Ư�� ������ �����ϴ� ��츦 �޾ƿͼ� SetbCanTrigger�� true�� Set���ְų�
	 * �� �� �ϳ��� ����� ����ϸ� ���� �� �ϴ�. */
	SetbCanTrigger(true);

	GLog->Log("ACTriggerVolume_Cannon::ACTriggerVolume_Cannon()");
}

void ACTriggerVolume_Cannon::OnBeginOverlap(class AActor* OverlappedActor, class AActor* OtherActor)
{
	Super::OnBeginOverlap(OverlappedActor, OtherActor);

	if (OnCannonEventTrigger.IsBound())
	{
		OnCannonEventTrigger.Broadcast(OverlappedActor, OtherActor);

		SetbCanTrigger(false);
	}
}

void ACTriggerVolume_Cannon::OnEndOverlap(class AActor* OverlappedActor, class AActor* OtherActor)
{
	Super::OnEndOverlap(OverlappedActor, OtherActor);

	if (GetbCanTrigger())
		SetbCanTrigger(false);
}