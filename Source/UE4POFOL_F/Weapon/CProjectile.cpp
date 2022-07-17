/* MEMO: CBullet.cpp 코드 배치 정리 완료 */
#include "Weapon/CProjectile.h"
#include "Global.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Enemy/CEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Player/CPlayer.h"
#include "Component/CWidgetComponent.h"

ACProjectile::ACProjectile()
{
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	RootComponent = Sphere;
	CHelpers::CreateComponent(this, &Mesh, "Mesh", Sphere);
	CHelpers::CreateActorComponent(this, &ProjectileComponent, "Projectile");
}

void ACProjectile::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentHit.AddDynamic(this, &ACProjectile::Hit);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ACProjectile::BeginOverlap);
}

ACProjectile* ACProjectile::SpawnProjectile(ACharacter* InSpawner, TSubclassOf<class ACProjectile> InProjectileClass, FName InSpawnSocketName)
{
	FActorSpawnParameters params;
	params.Owner = InSpawner;
	//params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	//
	//ACProjectile* projectile = InSpawner->GetWorld()->SpawnActor<ACProjectile>
	//(
	//	InProjectileClass, 
	//	InSpawner->GetMesh()->GetSocketLocation(InSpawnSocketName),
	//	FRotator::ZeroRotator,
	//	params
	//);
	//
	//projectile->SetOwner(InSpawner);
	//
	//if (projectile)
	//{
	//	UCameraComponent* camera = Cast<UCameraComponent>(InSpawner->GetComponentByClass(UCameraComponent::StaticClass()));
	//
	//	projectile->SetActorRotation(camera->GetComponentRotation());
	//	projectile->ShootProjectile(camera->GetForwardVector());
	//}

	return InSpawner->GetWorld()->SpawnActor<ACProjectile>
		(
			InProjectileClass,
			InSpawner->GetMesh()->GetSocketLocation(InSpawnSocketName),
			FRotator::ZeroRotator,
			params
		);
}

ACProjectile* ACProjectile::SpawnProjectile(ACharacter* InSpawner, TSubclassOf<class ACProjectile> InProjectileClass, FVector InSpawnLocation)
{
	FActorSpawnParameters params;
	params.Owner = InSpawner;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	//ACProjectile* projectile = InSpawner->GetWorld()->SpawnActor<ACProjectile>
	//(
	//	InProjectileClass, 
	//	InSpawner->GetMesh()->GetSocketLocation(InSpawnSocketName),
	//	FRotator::ZeroRotator,
	//	params
	//);
	//
	//projectile->SetOwner(InSpawner);
	//
	//if (projectile)
	//{
	//	UCameraComponent* camera = Cast<UCameraComponent>(InSpawner->GetComponentByClass(UCameraComponent::StaticClass()));
	//
	//	projectile->SetActorRotation(camera->GetComponentRotation());
	//	projectile->ShootProjectile(camera->GetForwardVector());
	//}

	return InSpawner->GetWorld()->SpawnActor<ACProjectile>
		(
			InProjectileClass,
			InSpawnLocation,
			FRotator::ZeroRotator,
			params
		);
}


void ACProjectile::ShootProjectile(const FVector& InDirection)
{
	ProjectileComponent->Velocity = InDirection * ProjectileComponent->InitialSpeed;
}

void ACProjectile::Hit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	CheckTrue(OtherActor == GetOwner());

	if (OtherActor->ActorHasTag("Enemy") == GetOwner()->ActorHasTag("Enemy"))
		return;

	float applyDamage = UKismetMathLibrary::RandomIntegerInRange(100.0f, 150.0f);

	UGameplayStatics::ApplyDamage(OtherActor, 100.0f, GetOwner()->GetInstigatorController(), this, NULL);
	
	GLog->Log("ACProjectile::Hit()");

	DestroyProjectile();
}

void ACProjectile::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CheckTrue(OtherActor == GetOwner());

	if (OtherActor->ActorHasTag("Enemy") == GetOwner()->ActorHasTag("Enemy"))
		return;

	float applyDamage = UKismetMathLibrary::RandomIntegerInRange(100.0f, 150.0f);

	UGameplayStatics::ApplyDamage(OtherActor, applyDamage, GetOwner()->GetInstigatorController(), this, NULL);

	GLog->Log("ACProjectile::BeginOverlap()");

	
	DestroyProjectile();
}

void ACProjectile::DestroyProjectile()
{
	Destroy();
}


