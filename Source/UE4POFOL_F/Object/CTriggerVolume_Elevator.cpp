#include "Object/CTriggerVolume_Elevator.h"
#include "Global.h"

ACTriggerVolume_Elevator::ACTriggerVolume_Elevator()
{
	/* TODO: GameState를 받아와서 특정 조건이 만족하면 SetbCanTrigger를 true로 Set해주거나,
	 * 인터페이스 클래스를 만들어서 Player가 특정 조건을 만족하는 경우를 받아와서 SetbCanTrigger를 true로 Set해주거나
	 * 둘 중 하나의 방법을 사용하면 좋을 듯 하다. */
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