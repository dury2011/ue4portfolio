#include "Enemy/CEnemy_Helix.h"
#include "Global.h"
#include "Components/CapsuleComponent.h"
#include "Component/CCharacterComponent.h"
#include "Weapon/CProjectile.h"

ACEnemy_Helix::ACEnemy_Helix()
{
	PrimaryActorTick.bCanEverTick = true;

	//GetCapsuleComponent()->SetRelativeScale3D(FVector(3.0f, 3.0f, 3.0f));

	USkeletalMesh* skeletalMesh;
	ConstructorHelpers::FObjectFinder<USkeletalMesh> asset(*FString("SkeletalMesh'/Game/ParagonMinions/Characters/Minions/Prime_Helix/Meshes/Prime_Helix.Prime_Helix'"));

	if (asset.Succeeded())
	{
		skeletalMesh = asset.Object;
		GetMesh()->SetSkeletalMesh(skeletalMesh);
		GetMesh()->SetupAttachment(GetCapsuleComponent());
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}

	ConstructorHelpers::FClassFinder<UAnimInstance> animInstance(*FString("AnimBlueprint'/Game/FORUE4POFOL/Enemy/Blueprint/ABP_Enemy_Helix.ABP_Enemy_Helix_C'"));

	if (animInstance.Succeeded())
	{
		AnimInstance = animInstance.Class;
		GetMesh()->SetAnimInstanceClass(AnimInstance);
	}

	ConstructorHelpers::FClassFinder<ACProjectile> assetClass(*FString("Blueprint'/Game/FORUE4POFOL/Weapon/BP_CProjectile_Helix.BP_CProjectile_Helix_C'"));

	if (assetClass.Succeeded())
		ProjectileClass = assetClass.Class;
	
	CharacterComponent = CreateDefaultSubobject<UCCharacterComponent>("CharacterComponent");
	CharacterComponent->SetCurrentStateType(EStateType::Idle);
}

void ACEnemy_Helix::BeginPlay()
{
	Super::BeginPlay();
}

void ACEnemy_Helix::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACEnemy_Helix::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACEnemy_Helix::OnAttack()
{
	CharacterComponent->SetIsMontagePlaying(true);
	
	int select = UKismetMathLibrary::RandomInteger(3);
	
	switch (select)
	{
		case 0:
		{
			CharacterComponent->GetActionDatasOnehand(0).PlayMontage(this);

			break;
		}
		case 1:
		{
			CharacterComponent->GetActionDatasOnehand(1).PlayMontage(this);

			break;
		}
		case 2:
		{
			CharacterComponent->GetActionDatasOnehand(2).PlayMontage(this);

			break;
		}
		case 3:
		{
			CharacterComponent->GetActionDatasOnehand(3).PlayMontage(this);

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

		Projectile = ACProjectile::SpawnProjectile
		(
			this, 
			ProjectileClass,
			projectileSpawnLocation
		);

		if (Projectile)
		{
			Projectile->SetOwner(this);
			Projectile->ShootProjectile(UKismetMathLibrary::GetDirectionUnitVector(projectileSpawnLocation, opponentLocation));
			Projectile->SetActorRotation(UKismetMathLibrary::GetDirectionUnitVector(projectileSpawnLocation, opponentLocation).Rotation());
		}
	}
}