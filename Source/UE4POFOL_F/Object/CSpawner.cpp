/* MEMO: CSpawner.cpp 코드 배치 정리 완료  */
#include "Object/CSpawner.h"
#include "Global.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Object/CTriggerVolume.h"
#include "Object/CTriggerVolume_Spawner.h"
#include "Components/BrushComponent.h"
#include "GameFramework/Character.h"
#include "Player/CPlayer.h"
#include "Components/WidgetComponent.h"

ACSpawner::ACSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>("CapsuleCollision");
	RootComponent = CapsuleCollision;
	//StaticMeshSpawner = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshSpanwer");
	StaticMeshSphere = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshSphere");
	StaticMeshSphere->SetupAttachment(CapsuleCollision);
	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("Health Bar Widget");
	HealthBarWidgetComponent->SetupAttachment(CapsuleCollision);

	//if (HealthBarWidgetComponent)
		//HealthBarWidgetComponent->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
	
	CapsuleCollision->OnComponentBeginOverlap.AddDynamic(this, &ACSpawner::OnBeginOverlap);
	CapsuleCollision->OnComponentEndOverlap.AddDynamic(this, &ACSpawner::OnEndOverlap);
	CapsuleCollision->OnComponentHit.AddDynamic(this, &ACSpawner::OnHit);
	
	//ConstructorHelpers::FObjectFinder<UStaticMeshComponent> assetSpawner(*FString("StaticMesh'/Game/ParagonProps/Monolith/Ruins/Meshes/Ruins_Player_Pedestal_02.Ruins_Player_Pedestal_02.Ruins_Player_Pedestal_02'"));
	//
	//if (assetSpawner.Succeeded())
	//	StaticMeshSpawner = assetSpawner.Object;
	//
	//ConstructorHelpers::FObjectFinder<UStaticMeshComponent> assetDeco(*FString("StaticMesh'/Game/ParagonProps/Agora/Props/Meshes/Agora_Deco_Portal_B.Agora_Deco_Portal_B'"));
	//
	//if (assetDeco.Succeeded())
	//	StaticMeshDeco = assetDeco.Object;
}

void ACSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> outActorArr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACTriggerVolume_Spawner::StaticClass(), outActorArr);

	for (int i = 0; i < outActorArr.Num(); i++)
		TriggerVolume = dynamic_cast<ACTriggerVolume_Spawner*>(outActorArr[i]);

	if (TriggerVolume)
		TriggerVolume->OnSpawnerEventTrigger.AddDynamic(this, &ACSpawner::ActivateSpawner);
}

void ACSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/* TODO: CSpawner의 체력이 5000.0f이하이면, 
	   기존 스폰되던 Enemy보다 더 높은 등급의 Enemy를 
	   기존 스폰 Enemy 중간 즈음에 Spawn 되도록 할 것*/
	//if (Hp <= 5000.0)
	//	GetWorldTimerManager().UnPauseTimer(SpawnerTimer);

	UpdateHitNumbers();
	CreateLineTrace();
}

float ACSpawner::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{	
	if (SpawnedEnemy < 30)
		return 0.0f;

	Damaged.DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser); // damage float
	//Damaged.DamageEvent = (FActionDamageEvent*)&DamageEvent;  // Hit 에니메이션 몽타주
	Damaged.EventInstigator = EventInstigator;
	Damaged.DamageCauser = DamageCauser; 
	Damaged.DamageAmount = DamageAmount;

	ShowHitNumber(Damaged.DamageAmount, GetActorLocation());
	ShowHealthBar();

	if (Damaged.EventInstigator == GetWorld()->GetFirstPlayerController())
		Hp -= Damaged.DamageAmount / 10.0f;
	else 
		Hp -= Damaged.DamageAmount;

	if (Hp <= 0.0f)
	{
		// TODO: 파괴되는 시네마틱 재생 (여기서 해 줄 수 있는지는 공부해야함)
		
		DestroySpawner();
	}

	return Damaged.DamageAmount; 
}

void ACSpawner::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void ACSpawner::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ACSpawner::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}

void ACSpawner::ActivateSpawner(class AActor* InOverlappedActor, class AActor* InOtherActor)
{
	//if (InOtherActor->GetClass() == ACPlayer::StaticClass())
		// 아래 코드 if 조건문 true, false 판단전에 InOtherActor Cast 하는 과정에서 InOtherActor가 Projectile이면 Controller가 없으므로 nullptr로 터짐
		//if (GetWorld()->GetFirstPlayerController() == Cast<ACharacter>(InOtherActor)->GetController())
		GetWorldTimerManager().SetTimer(SpawnerTimer, this, &ACSpawner::SpawnEnemy, SpawnDelaySecond, true);
}

void ACSpawner::CreateLineTrace()
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

void ACSpawner::SpawnEnemy()
{
	FVector spawnerFowardVector = GetActorForwardVector();
	FRotator spawnedEnemyRotation = UKismetMathLibrary::Conv_VectorToRotator(spawnerFowardVector);

	EnemySpawnParticleEffect();

	//if (EnemyNormalClass && EnemySpecialClass)
	//{

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

			if (enemy)
				SpawnedEnemy++;

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

			if (enemy)
				SpawnedEnemy++;

			GLog->Log("ACSpawner::SpawnEnemy() Special");
		}
	//}
}

void ACSpawner::DestroySpawner()
{
	GetWorldTimerManager().ClearTimer(SpawnerTimer);
	
	Destroy();
}

void ACSpawner::StoreHitNumber(UUserWidget* InHitNumber, FVector InLocation)
{
	HitNumbers.Add(InHitNumber, InLocation);

	FTimerHandle HitNumberTimer;
	FTimerDelegate HitNumberDelegate;
	HitNumberDelegate.BindUFunction(this, FName("DestroyHitNumber"), InHitNumber);
	GetWorld()->GetTimerManager().SetTimer(HitNumberTimer, HitNumberDelegate, 1.0f, false);
}

void ACSpawner::UpdateHitNumbers()
{
	for (auto& HitPair : HitNumbers)
	{
		UUserWidget* HitNumber{ HitPair.Key };
		const FVector Location{ HitPair.Value };
		FVector2D ScreenPosition;
		UGameplayStatics::ProjectWorldToScreen(GetWorld()->GetFirstPlayerController(), Location, ScreenPosition);
		HitNumber->SetPositionInViewport(ScreenPosition);
	}
}

void ACSpawner::DestroyHitNumber(UUserWidget* InHitNumber)
{
	HitNumbers.Remove(InHitNumber);
	InHitNumber->RemoveFromParent(); // 뷰포트에서 위젯을 지워줄 것이다.
}

void ACSpawner::ShowHealthBar()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(HealthBarTimer, this, &ACSpawner::HideHealthBar, 1.5f);

	if (HealthBarWidgetComponent)
		HealthBarWidgetComponent->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Visible);
}

void ACSpawner::HideHealthBar()
{
	if (HealthBarWidgetComponent)
		HealthBarWidgetComponent->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
}
