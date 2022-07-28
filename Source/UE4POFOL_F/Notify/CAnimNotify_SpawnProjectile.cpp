#include "Notify/CAnimNotify_SpawnProjectile.h"
#include "Global.h"
#include "Weapon/CProjectile.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Component/CWidgetComponent.h"

FString UCAnimNotify_SpawnProjectile::GetNotifyName_Implementation() const
{
	return FString("Spawn Projectile");
}

void UCAnimNotify_SpawnProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	CheckNull(MeshComp);
	CheckNull(ProjectileClass);

	ACharacter* character = Cast<ACharacter>(MeshComp->GetOwner());
	
	if (character)
	{
		UCameraComponent* cameraComponent = CHelpers::GetComponent<UCameraComponent>(character);
		UCWidgetComponent* widgetComponent = CHelpers::GetComponent<UCWidgetComponent>(character);
		
		Projectile = ACProjectile::SpawnProjectile(character, ProjectileClass, SpawnSocketName);

		if (Projectile)
		{
 			Projectile->SetOwner(character);

			if (cameraComponent)
				Projectile->SetActorRotation(cameraComponent->GetComponentRotation());

			if (CanActivateShoot)
			{
				if (!ShootSpawnedLocationFoward)
				{
					if (widgetComponent)
					{
						if (widgetComponent->GetHitResult().GetActor())
							Projectile->ShootProjectile(UKismetMathLibrary::GetDirectionUnitVector(character->GetMesh()->GetSocketLocation(SpawnSocketName), widgetComponent->GetHitResult().ImpactPoint));
						else
							Projectile->ShootProjectile(cameraComponent->GetForwardVector());
					}
				}
				else if (ShootSpawnedLocationFoward)
					Projectile->ShootProjectile(character->GetMesh()->GetSocketRotation(SpawnSocketName).Vector() * ShootSpeed);
			}
		}
	}
}