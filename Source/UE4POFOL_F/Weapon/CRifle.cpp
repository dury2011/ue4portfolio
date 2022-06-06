#include "Weapon/CRifle.h"
#include "Global.h"
#include "Player/CPlayer.h"
//#include "Weapon/CHUD.h"
#include "Weapon/CProjectile.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimMontage.h"
#include "Camera/CameraComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Materials/MaterialInstanceConstant.h"
//#include "Component/CWeaponComponent.h"

ACRifle::ACRifle()
	:bEquipped(false),
	 bAiming(false), 
	 bAutoFire(false), 
	 bEquipping(false),
	 FullAmmoCount(60),
	 FireAmmoCount(1),
	 LeftAmmoCount(61)
{
	PrimaryActorTick.bCanEverTick = true;
	
	CHelpers::CreateComponent<USkeletalMeshComponent>(this, &Mesh, "Mesh");
	CHelpers::GetAsset<UCurveFloat>(&AimCurve, "CurveFloat'/Game/FORUE4POFOL/Curve/Curve_Aim.Curve_Aim'");
	//CHelpers::GetClass<UMatineeCameraShake>(&CameraShakeClass, "Blueprint'/Game/BP_CameraShake.BP_CameraShake_C'");
	//CHelpers::GetAsset<USoundCue>(&FireSoundCue, "SoundCue'/Game/Sounds/S_RifleShoot_Cue.S_RifleShoot_Cue'");
	CHelpers::GetAsset<UParticleSystem>(&FlashParticle, "ParticleSystem'/Game/Scifi_Arsenal_Vol2/Particle_Systems/P_Sci-fi_Rifle_01_MuzzleFlash.P_Sci-fi_Rifle_01_MuzzleFlash'");
	//CHelpers::GetAsset<UParticleSystem>(&BulletParticle, "ParticleSystem'/Game/Particles_Rifle/Particles/VFX_Eject_bullet.VFX_Eject_bullet'");
	CHelpers::GetClass<ACProjectile>(&BulletClass, "Blueprint'/Game/FORUE4POFOL/Weapon/BP_CBullet.BP_CBullet_C'");
	//CHelpers::GetAsset<UMaterialInstanceConstant>(&Decal, "MaterialInstanceConstant'/Game/Materials/M_Decal_Inst.M_Decal_Inst'");
	//CHelpers::GetAsset<UParticleSystem>(&ImpactParticle, "ParticleSystem'/Game/Particles_Rifle/Particles/VFX_Impact_Default.VFX_Impact_Default'");

}

ACRifle* ACRifle::Spawn(UWorld* InWorld, ACharacter* InOwnerCharacter)
{
	FActorSpawnParameters params;
	
	params.Owner = InOwnerCharacter;

	return InWorld->SpawnActor<ACRifle>(params);
}

void ACRifle::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	//OwnerWeaponComponent = CHelpers::GetComponent<UCWeaponComponent>(OwnerCharacter);

	AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), HolsterSocket);

	OnTimelineFloat.BindUFunction(this, "Zooming");
	Timeline.AddInterpFloat(AimCurve, OnTimelineFloat);
	Timeline.SetPlayRate(200);

	ACPlayer* player = Cast<ACPlayer>(OwnerCharacter);
	
	if (!!player)
	{
		APlayerController* controller = player->GetController<APlayerController>();
		
		//if (!!controller) // controller가 있으면, 
			//HUD = controller->GetHUD<ACHUD>();	
	}
}

void ACRifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	Timeline.TickTimeline(DeltaTime);

	CheckFalse(bAiming);

	USpringArmComponent* springArm = CHelpers::GetComponent<USpringArmComponent>(OwnerCharacter);
	UCameraComponent* camera = CHelpers::GetComponent<UCameraComponent>(OwnerCharacter);
	FVector direction;
	FVector start;
	FVector end;

	if (!!springArm && !!camera)
	{
		direction = camera->GetForwardVector();

		FTransform transform = camera->GetComponentToWorld();
		start = transform.GetLocation() + direction;
		end = transform.GetLocation() + direction * AimDistance;
	}
	else
	{
		//TODO : Enemy 적은 controller가 AI이므로 springArm, camera 없음.
	}
}

void ACRifle::OnEquip()
{
	CheckTrue(bEquipping);
	CheckTrue(bEquipped);

	if (OnRifleFire.IsBound())
		OnRifleFire.Broadcast(FullAmmoCount, FireAmmoCount, LeftAmmoCount);

	bEquipping = true; 

	
	//if (bEquipped == true)
	//{
	//	OnUnequip();

	//	return;
	//}

	AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), HandSocket);
	
	bEquipped = true;
	bEquipping = false;
}

void ACRifle::OnUnequip()
{
	CheckTrue(bEquipping);
	CheckFalse(bEquipped);

	bEquipping = true;
	
	AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), HolsterSocket);
	
	bEquipped = false;
	bEquipping = false;
}

void ACRifle::Begin_Aim()
{
	CheckFalse(bEquipped);
	CheckTrue(bEquipping);

	USpringArmComponent* springArm = CHelpers::GetComponent<USpringArmComponent>(OwnerCharacter);
	UCameraComponent* camera = CHelpers::GetComponent<UCameraComponent>(OwnerCharacter);
	
	//if (!!HUD)
		//HUD->SetbDrawTrue();
	
	if (!!springArm && !!camera) //Player
	{
		bAiming = true;
	
		springArm->TargetArmLength = 100;
		springArm->SocketOffset = FVector(0, 30, 0);
		springArm->bEnableCameraLag = false;

		Timeline.PlayFromStart();

		return;
	}
	else
	{
		bAiming = true;		
		
		return;
	}
	
}

void ACRifle::End_Aim()
{
	CheckFalse(bEquipped);
	CheckTrue(bEquipping);
		
	USpringArmComponent* springArm = CHelpers::GetComponent<USpringArmComponent>(OwnerCharacter);
	UCameraComponent* camera = CHelpers::GetComponent<UCameraComponent>(OwnerCharacter);
	
	//if (!!HUD)
		//HUD->SetbDrawFalse();

	if (!!springArm && !!camera) //Player
	{
		bAiming = false;



		springArm->TargetArmLength = 200;
		springArm->SocketOffset = FVector(0, 60, 0);
		springArm->bEnableCameraLag = true;

		Timeline.ReverseFromEnd();

		return;
	}
	else
	{
		bAiming = false;

		return;
	}

}

void ACRifle::Zooming(float Output)
{
	UCameraComponent* camera = CHelpers::GetComponent<UCameraComponent>(OwnerCharacter);
	CheckNull(camera);

	camera->FieldOfView = Output;
}

void ACRifle::ToggleAutoFire()
{
	CheckTrue(bFiring);

	bAutoFire = !bAutoFire;
}

void ACRifle::Begin_Fire()
{
	CheckFalse(bEquipped);
	CheckTrue(bEquipping);
	CheckFalse(bAiming);
	CheckTrue(bFiring);

	bFiring = true;
	PitchAngle = 0;
	
	if (bAutoFire)
	{
		GetWorld()->GetTimerManager().SetTimer(FiringTimer, this, &ACRifle::Firing, AutoFireInterval, true, 0);

		return;
	}

	Firing();
}

void ACRifle::Firing()
{
	if (OnRifleFire.IsBound())
	{
		LeftAmmoCount -= FireAmmoCount;
		OnRifleFire.Broadcast(FullAmmoCount, FireAmmoCount, LeftAmmoCount);
	}
	
	USpringArmComponent* springArm = CHelpers::GetComponent<USpringArmComponent>(OwnerCharacter);
	UCameraComponent* camera = CHelpers::GetComponent<UCameraComponent>(OwnerCharacter);

	FVector direction;
	FVector start;
	FVector end;

	if (!!springArm && !!camera)
	{
		direction = camera->GetForwardVector(); // 카메라 전방 백터 
		
		FTransform transform = camera->GetComponentToWorld();

		start = transform.GetLocation() + direction;
		direction = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(direction, 0.2f);
		end = transform.GetLocation() + direction * AimDistance;
	}
	else
	{
		//TODO : Enemy 적은 controller가 AI 이므로 그에 맞는 기능을 구현해야함. 
		direction = OwnerCharacter->GetActorForwardVector();
		
		FTransform transform = OwnerCharacter->GetActorTransform();
		
		start = transform.GetLocation() + direction;
		direction = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(direction, 0.2f);
		end = transform.GetLocation() + direction * AimDistance;
	}
	
	////CameraShake
	//{
	//	ACPlayer* player = Cast<ACPlayer>(OwnerCharacter);
	//	if (!!player)
	//	{
	//		APlayerController* controller = player->GetController<APlayerController>();
	//		if (!!controller && !!CameraShakeClass)
	//			controller->PlayerCameraManager->StartCameraShake(CameraShakeClass);
	//	}
	//}

	FVector muzzleLocation = Mesh->GetSocketLocation("MuzzleFlash");

	////Play Sound Cue
	//{
	//	if (!!FireSoundCue)
	//		UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSoundCue, muzzleLocation);
	//}

	//Play Flash / Bullet Paricle
	{
		UGameplayStatics::SpawnEmitterAttached(FlashParticle, Mesh, "MuzzleFlash", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset);		
	}

	//Spawn Bullet
	{
		if (!!BulletClass)
		{
			FVector spawnLocation = muzzleLocation + direction * 100;
			ACProjectile* bullet = GetWorld()->SpawnActor<ACProjectile>(BulletClass, spawnLocation, direction.Rotation());
			bullet->ShootProjectile(direction);	
		}
	}

	//FirePlayMontage
	{
		//OwnerCharacter->PlayAnimMontage(OwnerWeaponComponent->GetWeaponData(2)->GetDoActionData(0).Montage, 2.0f);
	}

	//Pitch Angle Firing Called 되면 반동 주는 부분.
	{
		PitchAngle -= LimitPitchAngle * GetWorld()->GetDeltaSeconds(); // Firing Call 될 때 마다 PitchAngle = PitchAngle - LimitiedPitchAngle(0.25f 즉, 45도)
		
		if (PitchAngle > -LimitPitchAngle)
			OwnerCharacter->AddControllerPitchInput(PitchAngle);
	}

	TArray<AActor *> ignoreActors;
	ignoreActors.Add(OwnerCharacter);
	FHitResult hitResult;

	//Decal & Play Impact Particle
	//if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), start, end, ETraceTypeQuery::TraceTypeQuery1, false, ignoreActors, EDrawDebugTrace::None, hitResult, true))
	//{
	//	FRotator rotator = hitResult.ImpactNormal.Rotation();
	//	UGameplayStatics::SpawnDecalAtLocation(GetWorld(), Decal, FVector(5), hitResult.Location, rotator, 10);
	//	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, hitResult.Location, rotator, true);
	//}

	if (UKismetSystemLibrary::LineTraceSingle(GetWorld(), start, end, ETraceTypeQuery::TraceTypeQuery3, false, ignoreActors, EDrawDebugTrace::None, hitResult, true))
	{
		AStaticMeshActor* actor = Cast<AStaticMeshActor>(hitResult.GetActor());
		if (!!actor)
		{
			UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(actor->GetRootComponent());
			if (!!mesh && mesh->BodyInstance.bSimulatePhysics)
			{
				direction = actor->GetActorLocation() - OwnerCharacter->GetActorLocation();
				direction.Normalize();

				mesh->AddImpulseAtLocation(direction * mesh->GetMass() * 100, OwnerCharacter->GetActorLocation());

				return;
			}
		}
	}
}

void ACRifle::End_Fire()
{
	bFiring = false;

	if (bAutoFire)
		GetWorld()->GetTimerManager().ClearTimer(FiringTimer);
}

void ACRifle::ChangeRifle()
{
	bEquipped = false;

	AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), HolsterSocket);
}

