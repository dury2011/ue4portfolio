/* MEMO: CBullet.cpp 코드 배치 정리 완료 22.08 */
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
#include "Enemy/CEnemy_Rifle.h"

// public: //////////////////////////////////////////////////////////////////////
ACProjectile::ACProjectile()
{
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	RootComponent = Sphere;
	CHelpers::CreateComponent(this, &Mesh, "Mesh", Sphere);
	CHelpers::CreateActorComponent(this, &ProjectileComponent, "Projectile");
}

ACProjectile* ACProjectile::SpawnProjectile(ACharacter* InSpawner, TSubclassOf<class ACProjectile> InProjectileClass, FName InSpawnSocketName)
{
	FActorSpawnParameters params;
	params.Owner = InSpawner;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
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

void ACProjectile::DestroyProjectile()
{
	Destroy();
}

// private: //////////////////////////////////////////////////////////////////////
void ACProjectile::Hit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// MEMO: Hit 사용 안함
	//CheckTrue(OtherActor == GetOwner());
	//
	//if (OtherActor->ActorHasTag("Enemy") == GetOwner()->ActorHasTag("Enemy"))
	//	return;
	//
	//float applyDamage = UKismetMathLibrary::RandomIntegerInRange(100.0f, 150.0f);
	//
	//UGameplayStatics::ApplyDamage(OtherActor, 100.0f, GetOwner()->GetInstigatorController(), this, NULL);
	//
	//GLog->Log("ACProjectile::Hit()");
	//
	////DestroyProjectile();
}

void ACProjectile::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GLog->Log("ACProjectile::BeginOverlap()");
	CheckTrue(OtherActor == GetOwner());

	float applyDamage = AttackDamage + UKismetMathLibrary::RandomIntegerInRange(0, AttackDamageDeviation);
	
	if (IsCannonRangedProjectile && OtherActor->ActorHasTag("Enemy"))
	{
		ACEnemy_Rifle* enemyRifle = Cast<ACEnemy_Rifle>(OtherActor);

		if (enemyRifle)
			enemyRifle->TakeDamageAction_CannonRangedProjectile(applyDamage);

		return;
	}

	if (OtherActor->ActorHasTag("Enemy") && GetOwner()->ActorHasTag("Player"))
	{

		UGameplayStatics::ApplyDamage(OtherActor, applyDamage, GetOwner()->GetInstigatorController(), this, NULL);

		GLog->Log("ACProjectile::BeginOverlap()");
	}
}

// protected: //////////////////////////////////////////////////////////////////////
void ACProjectile::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentHit.AddDynamic(this, &ACProjectile::Hit);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ACProjectile::BeginOverlap);
}