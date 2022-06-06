#include "StaticObject/CTriggerVolume.h"
#include "Global.h"

ACTriggerVolume::ACTriggerVolume()
{
	OnActorBeginOverlap.AddDynamic(this, &ACTriggerVolume::OnBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ACTriggerVolume::OnEndOverlap);
}

void ACTriggerVolume::BeginPlay()
{
	Super::BeginPlay();

	DrawDebugBox(GetWorld(), GetActorLocation(), GetActorScale() * 100, FColor::Cyan, true, -1, 0, 5);
}

void ACTriggerVolume::OnBeginOverlap(class AActor* OverlappedActor, class AActor* OtherActor)
{	
	CheckFalse(bCanTrigger);
	CheckNull(OtherActor);
	CheckTrue(OtherActor == this);
	
	GLog->Log("ACTriggerVolume::OnBeginOverlap()");
}

void ACTriggerVolume::OnEndOverlap(class AActor* OverlappedActor, class AActor* OtherActor)
{
	CheckFalse(bCanTrigger);
	CheckNull(OtherActor);
	CheckTrue(OtherActor == this);

	GLog->Log("ACTriggerVolume::OnEndOverlap()");
}
