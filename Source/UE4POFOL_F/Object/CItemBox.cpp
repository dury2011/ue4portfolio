#include "Object/CItemBox.h"
#include "Global.h"
#include "Components/SphereComponent.h"
#include "Object/CItem.h"

ACItemBox::ACItemBox()
{
	PrimaryActorTick.bCanEverTick = true;
	SphereCollision = CreateDefaultSubobject<USphereComponent>("Sphere Collision");
	RootComponent = SphereCollision;
}

void ACItemBox::BeginPlay()
{
	Super::BeginPlay();

}

void ACItemBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ACItemBox::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	SpawnDamageEffect();
	
	Hp -= DamageAmount;

	if (Hp <= 0.0f)
	{
		SpawnDestroyEffect();

		if (HavingItemClass)
			Item = ACItem::SpawnItem(this, HavingItemClass, GetActorTransform());

		Destroy();
	}

	return DamageAmount;
}







