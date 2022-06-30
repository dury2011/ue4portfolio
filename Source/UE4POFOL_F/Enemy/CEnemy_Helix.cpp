#include "Enemy/CEnemy_Helix.h"
#include "Global.h"
#include "Components/CapsuleComponent.h"
#include "Component/CCharacterComponent.h"
#include "Weapon/CProjectile.h"

ACEnemy_Helix::ACEnemy_Helix()
{
	PrimaryActorTick.bCanEverTick = true;

	USkeletalMesh* skeletalMesh;
	ConstructorHelpers::FObjectFinder<USkeletalMesh> asset(*FString("SkeletalMesh'/Game/ParagonMinions/Characters/Minions/Prime_Helix/Meshes/Prime_Helix.Prime_Helix'"));

	if (asset.Succeeded())
	{
		skeletalMesh = asset.Object;
		GetMesh()->SetSkeletalMesh(skeletalMesh);
		GetMesh()->SetupAttachment(GetCapsuleComponent());
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
		GetMesh()->SetAnimInstanceClass(AnimBlueprint);
	}
}

void ACEnemy_Helix::BeginPlay()
{
	Super::BeginPlay();
}

void ACEnemy_Helix::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACEnemy_Helix::OnAttack()
{
	int select = UKismetMathLibrary::RandomInteger(3);
	
	switch (select)
	{
		Super::OnAttack();
		
		case 0:
		{
			ActionDatas[0].PlayMontage(this);

			break;
		}
		case 1:
		{
			ActionDatas[1].PlayMontage(this);

			break;
		}
		case 2:
		{
			ActionDatas[2].PlayMontage(this);

			break;
		}
		case 3:
		{
			ActionDatas[3].PlayMontage(this);

			break;
		}
		default:
			break;
	}
	
	GLog->Log("ACEnemy_Helix::OnAttack()");
}

void ACEnemy_Helix::SpawnHelixProjectile()
{
	if (GetOpponent())
	{
		FVector opponentLocation = GetOpponent()->GetActorLocation();
		FVector projectileSpawnLocation = FVector(GetOpponent()->GetActorLocation().X, GetOpponent()->GetActorLocation().Y, GetOpponent()->GetActorLocation().Z + 800.0f);

		ACProjectile* projectile = ACProjectile::SpawnProjectile
		(
			this, 
			ProjectileClass[0],
			projectileSpawnLocation
		);

		if (projectile)
		{
			projectile->SetOwner(this);
			projectile->ShootProjectile(UKismetMathLibrary::GetDirectionUnitVector(projectileSpawnLocation, opponentLocation));
			projectile->SetActorRotation(UKismetMathLibrary::GetDirectionUnitVector(projectileSpawnLocation, opponentLocation).Rotation());
		}
	}
}