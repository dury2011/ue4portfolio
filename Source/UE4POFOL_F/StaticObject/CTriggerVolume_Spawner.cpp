#include "StaticObject/CTriggerVolume_Spawner.h"
#include "Global.h"
#include "Weapon/CProjectile.h"

ACTriggerVolume_Spawner::ACTriggerVolume_Spawner()
{
	/* TODO: GameState를 받아와서 특정 조건이 만족하면 SetbCanTrigger를 true로 Set해주거나,
	 * 인터페이스 클래스를 만들어서 Player가 특정 조건을 만족하는 경우를 받아와서 SetbCanTrigger를 true로 Set해주거나 
	 * 둘 중 하나의 방법을 사용하면 좋을 듯 하다. */
	SetbCanTrigger(true);

	GLog->Log("ACTriggerVolume_Spawner::ACTriggerVolume_Spawner()");
}

void ACTriggerVolume_Spawner::OnBeginOverlap(class AActor* OverlappedActor, class AActor* OtherActor)
{
	if (OtherActor->GetClass() == ACProjectile::StaticClass())
		return;
	
	Super::OnBeginOverlap(OverlappedActor, OtherActor);

	if (OnSpawnerEventTrigger.IsBound())
	{
		OnSpawnerEventTrigger.Broadcast(OverlappedActor, OtherActor);

		SetbCanTrigger(false);
	}
}

void ACTriggerVolume_Spawner::OnEndOverlap(class AActor* OverlappedActor, class AActor* OtherActor)
{
	if (OtherActor->GetClass() == ACProjectile::StaticClass())
		return;
	
	Super::OnEndOverlap(OverlappedActor, OtherActor);

	if (GetbCanTrigger())
		SetbCanTrigger(false);
}
