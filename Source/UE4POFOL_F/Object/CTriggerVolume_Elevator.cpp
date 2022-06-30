#include "Object/CTriggerVolume_Elevator.h"
#include "Global.h"

ACTriggerVolume_Elevator::ACTriggerVolume_Elevator()
{
	/* TODO: GameState�� �޾ƿͼ� Ư�� ������ �����ϸ� SetbCanTrigger�� true�� Set���ְų�,
	 * �������̽� Ŭ������ ���� Player�� Ư�� ������ �����ϴ� ��츦 �޾ƿͼ� SetbCanTrigger�� true�� Set���ְų�
	 * �� �� �ϳ��� ����� ����ϸ� ���� �� �ϴ�. */
	SetbCanTrigger(true);

	GLog->Log("ACTriggerVolume_Cannon::ACTriggerVolume_Elevator()");
}

void ACTriggerVolume_Elevator::OnBeginOverlap(class AActor* OverlappedActor, class AActor* OtherActor)
{
	Super::OnBeginOverlap(OverlappedActor, OtherActor);

	if (OnElevatorEventTrigger.IsBound())
	{
		OnElevatorEventTrigger.Broadcast(OverlappedActor, OtherActor);

		SetbCanTrigger(false);
	}
}

void ACTriggerVolume_Elevator::OnEndOverlap(class AActor* OverlappedActor, class AActor* OtherActor)
{
	Super::OnEndOverlap(OverlappedActor, OtherActor);

	if (GetbCanTrigger())
		SetbCanTrigger(false);
}