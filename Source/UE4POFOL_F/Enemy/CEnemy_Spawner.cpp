#include "Enemy/CEnemy_Spawner.h"
#include "Global.h"
#include "Object/CTriggerVolume_Spawner.h"

// public: //////////////////////////////////////////////////////////////////////
ACEnemy_Spawner::ACEnemy_Spawner()
{
	StaticMeshSpawner = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshSpanwer");
	StaticMeshSphere = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshSphere");
	StaticMeshSpawner->SetupAttachment(RootComponent);
	StaticMeshSphere->SetupAttachment(StaticMeshSpawner);
}

void ACEnemy_Spawner::Tick(float DeltaTime)
{
	// Super::Tick(DeltaTime); 필요 없을 듯?

	CreateLineTrace();
}

void ACEnemy_Spawner::OnAttack()
{
	// Super::OnAttack(); 쓸지 안쓸지 모르겠다. 스포너는 에니메이션이 없다...
}

float ACEnemy_Spawner::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	// ActivateDamageEffect(false, EWeaponType::Unarmed);
	
	Hp -= DamageAmount;

	return DamageAmount;
}

// private: //////////////////////////////////////////////////////////////////////
void ACEnemy_Spawner::ActivateEnemySpawner(class AActor* InOverlappedActor, class AActor* InOtherActor)
{
	GetWorldTimerManager().SetTimer(SpawnerTimer, this, &ACEnemy_Spawner::SpawnEnemy, EnemySpawnDelaySecond, true);
}

void ACEnemy_Spawner::CreateLineTrace()
{
	FTransform staticMeshTransform = StaticMeshSphere->GetComponentToWorld();
	FVector lineTraceStart = staticMeshTransform.GetLocation();
	FRotator staticMeshRotator = FRotator
	(
		staticMeshTransform.GetRotation().Rotator().Pitch - 45.0f,
		staticMeshTransform.GetRotation().Rotator().Yaw,
		staticMeshTransform.GetRotation().Rotator().Roll
	);

	//FVector lineTraceEnd = lineTraceStart + FQuat(staticMeshTransform.GetRotation()).GetForwardVector() * 3000.0f;
	FVector lineTraceEnd = lineTraceStart + FQuat(staticMeshRotator).GetForwardVector() * 3000.0f;

	TArray<AActor*>lineTraceIgnoreActors;
	//TEnumAsByte<EDrawDebugTrace::Type> drawDebugType;

	lineTraceIgnoreActors.Add(this);

	UKismetSystemLibrary::LineTraceSingle
	(
		GetWorld(),
		lineTraceStart,
		lineTraceEnd,
		ETraceTypeQuery::TraceTypeQuery4,
		false,
		lineTraceIgnoreActors,
		EDrawDebugTrace::ForOneFrame,
		HitResult,
		true,
		FColor(1, 0, 0, 1),
		FLinearColor::Green,
		0.0f
	);
}

void ACEnemy_Spawner::SpawnEnemy()
{
	FVector spawnerFowardVector = GetActorForwardVector();
	FRotator spawnedEnemyRotation = UKismetMathLibrary::Conv_VectorToRotator(spawnerFowardVector);

	FActorSpawnParameters params;
	params.Owner = this;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	if (Hp > 5000.0f)
	{
		ACEnemy* enemy = GetWorld()->SpawnActor<ACEnemy>
			(
				EnemyNormalClass,
				FVector(HitResult.ImpactPoint.X, HitResult.ImpactPoint.Y, HitResult.ImpactPoint.Z + 150.0f),
				spawnedEnemyRotation,
				params
			);

		GLog->Log("ACSpawner::SpawnEnemy() Normal");
	}
	else
	{
		ACEnemy* enemy = GetWorld()->SpawnActor<ACEnemy>
			(
				EnemySpecialClass,
				FVector(HitResult.ImpactPoint.X, HitResult.ImpactPoint.Y, HitResult.ImpactPoint.Z + 150.0f),
				spawnedEnemyRotation,
				params
			);

		GLog->Log("ACSpawner::SpawnEnemy() Special");
	}
}

void ACEnemy_Spawner::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Super::OnBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult); 이 경우도 쓸지 안 쓸지 모르겠다...
}

void ACEnemy_Spawner::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ACEnemy_Spawner::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}

// protected: //////////////////////////////////////////////////////////////////////
void ACEnemy_Spawner::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> outActorArr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACTriggerVolume_Spawner::StaticClass(), outActorArr);

	for (int i = 0; i < outActorArr.Num(); i++)
		TriggerVolume = dynamic_cast<ACTriggerVolume_Spawner*>(outActorArr[i]);

	if (TriggerVolume)
		TriggerVolume->OnSpawnerEventTrigger.AddDynamic(this, &ACEnemy_Spawner::ActivateEnemySpawner);
}
