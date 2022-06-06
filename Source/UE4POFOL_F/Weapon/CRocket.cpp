/* MEMO: CRocket.cpp 코드 배치 정리 완료 */
#include "Weapon/CRocket.h"
#include "Global.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Player/CPlayer.h"
#include "Particles/ParticleSystem.h"

#define LOG_ACROCKET

ACRocket::ACRocket()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateComponent(this, &Sphere, "Capsule");
	CHelpers::CreateComponent(this, &Mesh, "Mesh", Sphere);

	CHelpers::CreateActorComponent(this, &Projectile, "ProjectileComponent");

	UStaticMesh* staticMesh;
	CHelpers::GetAsset<UStaticMesh>(&staticMesh, "StaticMesh'/Game/Bullet/Sphere.Sphere'");
	Mesh->SetStaticMesh(staticMesh);

	RootComponent = Sphere;
}

void ACRocket::BeginPlay()
{
	Super::BeginPlay();
	
	//Capsule->OnComponentHit.AddDynamic(this, &ACRocket::Hit);
	
	Sphere->OnComponentHit.AddDynamic(this, &ACRocket::Hit);

	SetLifeSpan(3.5);
			
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Player", LockedOnActors);
}

void ACRocket::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CreateLineTraceToLand();
	CheckLockedOn();
	SpawnMarkNiagara();

#ifdef LOG_ACROCKET
	CLog::Print(DistanceToLand, 7);
#endif LOG_ACROCKET
}

void ACRocket::Shoot(const FVector& InDirection)
{
	Projectile->Velocity = InDirection * Projectile->InitialSpeed;
}

void ACRocket::CreateLineTraceToLand()
{
	FVector lineTraceStart = GetActorLocation();
	FVector lineTraceEnd = lineTraceStart + FVector(0.0f, 0.0f, -6000.0f);
	TArray<AActor*> lineTraceIgnoreActor;
	TEnumAsByte<EDrawDebugTrace::Type> drawDebugType = EDrawDebugTrace::ForDuration;

	lineTraceIgnoreActor.Add(this);

	UKismetSystemLibrary::LineTraceSingle
	(
		GetWorld(),
		lineTraceStart,
		lineTraceEnd,
		ETraceTypeQuery::TraceTypeQuery4,
		false,
		lineTraceIgnoreActor,
		drawDebugType,
		HitResult,
		true,
		FColor(1, 0, 0, 1),
		FLinearColor::Green,
		0.0f
	);
}

void ACRocket::CheckLockedOn()
{
	FHitResult hitResult = HitResult;

	if (hitResult.bBlockingHit)
	{
		DistanceToLand = hitResult.Distance;
		ImpactPointToLand = hitResult.ImpactPoint;

		if (DistanceToLand >= 1500.0f)
			LockedOn();
		else
			return;
	}
}

void ACRocket::LockedOn()
{
	CheckTrue(bLockedOn);
	
	for (AActor* targetActor : LockedOnActors)
	{
		if (targetActor)
		{
			bLockedOn = true;

			ACPlayer* targetPlayer = Cast<ACPlayer>(targetActor);
			
			FVector targetVector = targetPlayer->GetActorLocation() - GetActorLocation();

			FLatentActionInfo latentInfo;
			latentInfo.CallbackTarget = this;

			UKismetSystemLibrary::MoveComponentTo
			(
				GetRootComponent(),
				targetPlayer->GetActorLocation(),
				targetVector.Rotation(),
				false,
				false,
				0.35f,
				false,
				EMoveComponentAction::Move,
				latentInfo
			);
		}
	}
}

void ACRocket::SpawnMarkNiagara()
{
	//CheckTrue(bLockedOn);
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation
	(
		GetWorld(),
		RocketLocMark,
		ImpactPointToLand
	);

}

void ACRocket::Hit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	bLockedOn = false;

	ACPlayer* player = Cast<ACPlayer>(OtherActor);

	FActionDamageEvent damageDatas;
	damageDatas.DamageData = &RocketDamageDatas[0];

	if (OtherActor == player)
		OtherActor->TakeDamage(100.f, damageDatas, Cast<AController>(GetOwner()), this);

	UGameplayStatics::SpawnEmitterAtLocation
	(
		GetWorld(),
		Explosion,
		GetActorLocation()
	);

	GLog->Log("Rocket Hit");

	Destroy();
}

