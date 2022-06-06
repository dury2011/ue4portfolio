/* MEMO: CBall.cpp 코드 배치 정리 완료 */
#include "Weapon/CBall.h"
#include "Global.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Player/CPlayer.h"

#define LOG_ACBALL

ACBall::ACBall()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateComponent(this, &Capsule, "Capsule");
	CHelpers::CreateComponent(this, &Mesh, "Mesh", Capsule);

	CHelpers::CreateActorComponent(this, &Projectile, "ProjectileComponent");

	UStaticMesh* staticMesh;
	CHelpers::GetAsset<UStaticMesh>(&staticMesh, "StaticMesh'/Game/Bullet/Sphere.Sphere'");
	Mesh->SetStaticMesh(staticMesh);

	//Capsule->AddRelativeLocation(FVector(FVector::ZeroVector));
	//UMaterialInstanceConstant* material;
	// CHelpers::GetAsset<UMaterialInstanceConstant>(&material, "");
}

void ACBall::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(5.0f);

	//Projectile->bIsHomingProjectile = true;
	//Projectile->HomingTargetComponent = GetWorld()->GetFirstPlayerController()->GetRootComponent();


	GetWorldTimerManager().SetTimer(Timer, this, &ACBall::SpawnMarkNiagara, 0.3f, true);
	
	Capsule->OnComponentHit.AddDynamic(this, &ACBall::Hit);
}

void ACBall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CreateLineTraceToLand();

#ifdef LOG_ACBALL
//	CLog::Print(GetWorld()-> GetFirstPlayerController()->GetRootComponent()->GetName(), 8);
//	CLog::Print(*Projectile->HomingTargetComponent->GetName(),7);
#endif LOG_ACBALL
}

void ACBall::Shoot(const FVector& InDirection)
{
	Projectile->Velocity = InDirection * Projectile->InitialSpeed;
}

void ACBall::CreateLineTraceToLand()
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

	FHitResult hitResult = HitResult;

	if (hitResult.bBlockingHit)
	{
		DistanceToLand = hitResult.Distance;
		ImpactPointToLand = hitResult.ImpactPoint;
	}
}

void ACBall::SpawnMarkNiagara()
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation
	(
		GetWorld(),
		BallLocMark,
		ImpactPointToLand
	);
}

void ACBall::Hit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ACPlayer* player = Cast<ACPlayer>(OtherActor);

	//FActionDamageEvent hitDatas;
	//hitDatas.HitData = &BallHitDatas[0];

	//if (OtherActor == player)
	//	OtherActor->TakeDamage(100.f, hitDatas, Cast<AController>(GetOwner()), this);
	
	GLog->Log("Ball Hit");
	
	//Destroy();
}

