#include "Object/CItem.h"
#include "Global.h"
#include "Components/SphereComponent.h"
#include "Player/CPlayer.h"

ACItem::ACItem()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>("Sphere Collision");
	RootComponent = SphereCollision;
	SetCurrentItemType((EItemType)UKismetMathLibrary::RandomInteger64InRange(0, 2));
}

void ACItem::BeginPlay()
{
	Super::BeginPlay();

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ACItem::OnBeginOverlap);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &ACItem::OnEndOverlap);
}

void ACItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (EatActor)
		//UpdateDestroyEffect();
}

ACItem* ACItem::SpawnItem(AActor* InSpawner, TSubclassOf<class ACItem> InItemClass, const FTransform& InSpawnTransform)
{
	FActorSpawnParameters params;
	params.Owner = InSpawner;

	return InSpawner->GetWorld()->SpawnActor<ACItem>(InItemClass, InSpawnTransform, params);
}

//FTransform ACItem::UpdateDestroyEffect_Implementation()
//{
//	return Transform = EatActor->GetActorTransform();
//}

void ACItem::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ICInterface_PlayerState* interfacePlayer = Cast<ICInterface_PlayerState>(OtherActor);

	if(interfacePlayer)
	{	
		EatActor = OtherActor;
		
		SpawnDestroyEffect();
		Destroy();
	}
}

void ACItem::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ACItem::SetCurrentItemType(EItemType InType)
{
	ItemType = InType;
}

EItemType ACItem::GetCurrentItemType()
{
	return ItemType;
}

float ACItem::ActivateItemAbility()
{
	if (ItemType == EItemType::Hp)
	{
		return 500.0f;
	}
	else if (ItemType == EItemType::Mp)
	{
		return 500.0f;
	}
	else if (ItemType == EItemType::Sp)
	{
		return 500.0f;
	}
	else
		return 0.0f;
}
