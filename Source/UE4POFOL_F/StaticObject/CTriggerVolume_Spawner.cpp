#include "StaticObject/CTriggerVolume_Spawner.h"
#include "Global.h"
#include "Weapon/CProjectile.h"

ACTriggerVolume_Spawner::ACTriggerVolume_Spawner()
{
	/* TODO: GameState�� �޾ƿͼ� Ư�� ������ �����ϸ� SetbCanTrigger�� true�� Set���ְų�,
	 * �������̽� Ŭ������ ���� Player�� Ư�� ������ �����ϴ� ��츦 �޾ƿͼ� SetbCanTrigger�� true�� Set���ְų� 
	 * �� �� �ϳ��� ����� ����ϸ� ���� �� �ϴ�. */
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
