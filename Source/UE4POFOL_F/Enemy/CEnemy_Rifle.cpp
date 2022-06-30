/* MEMO: ACEnemy_Rifle.cpp 코드 배치 정리 완료  */
#include "Enemy/CEnemy_Rifle.h"
#include "Global.h"
#include "Components/CapsuleComponent.h"
#include "Component/CCharacterComponent.h"
#include "Weapon/CProjectile.h"
#include "Object/CTriggerVolume_Spawner.h"

//#define MaxSplineCount 5

ACEnemy_Rifle::ACEnemy_Rifle()
{
	PrimaryActorTick.bCanEverTick = true;
	
	USkeletalMesh* skeletalMesh;
	ConstructorHelpers::FObjectFinder<USkeletalMesh> asset(*FString("SkeletalMesh'/Game/ParagonMinions/Characters/Minions/Down_Minions/Meshes/Minion_Lane_Siege_Core_Dawn.Minion_Lane_Siege_Core_Dawn'"));

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

void ACEnemy_Rifle::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> outActorArr;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACTriggerVolume_Spawner::StaticClass(), outActorArr);

	for (int i = 0; i < outActorArr.Num(); i++)
		TriggerVolumeSpanwer = dynamic_cast<ACTriggerVolume_Spawner*>(outActorArr[i]);

	//Index = UKismetMathLibrary::RandomInteger(MaxSplineCount);
} 

void ACEnemy_Rifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACEnemy_Rifle::SpawnAndShootProjectile()
{
	ACProjectile* projectile = ACProjectile::SpawnProjectile(this, ProjectileClass[0], FName("Muzzle_Front"));
	
	if (projectile)
	{
		projectile->SetOwner(this);
		projectile->SetActorRotation(GetActorRotation());
		projectile->ShootProjectile(UKismetMathLibrary::GetDirectionUnitVector(GetMesh()->GetSocketLocation(FName("Muzzle_Front")), GetOpponent()->GetActorLocation()));
	}
}

void ACEnemy_Rifle::OnAttack()
{
	if (GetOpponent())
	{
		Super::OnAttack();
		
		ActionDatas[0].PlayMontage(this);

		GLog->Log("ACEnemy_Rifle::OnAttack()");
	}
}

void ACEnemy_Rifle::GetMoveToLocation(FVector& OutLocation)
{
	if (TriggerVolumeSpanwer)
		OutLocation = TriggerVolumeSpanwer->GetActorLocation();
}

void ACEnemy_Rifle::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	//CharacterComponent->SetCurrentStateType(EStateType::Damage);
}

void ACEnemy_Rifle::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void ACEnemy_Rifle::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
	
	//CharacterComponent->SetCurrentStateType(EStateType::Damage);
}





