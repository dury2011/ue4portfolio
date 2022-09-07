#include "Enemy/CEnemy_Boss.h"
#include "Global.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Interface/CInterface_PlayerState.h"
#include "Player/CPlayer.h"
#include "NavigationSystem.h"
#include "Weapon/CProjectile.h"

ACEnemy_Boss::ACEnemy_Boss()
{
	USkeletalMesh* skeletalMesh;
	ConstructorHelpers::FObjectFinder<USkeletalMesh> asset(*FString("SkeletalMesh'/Game/ParagonGreystone/Characters/Heroes/Greystone/Skins/Tough/Meshes/Greystone_Tough.Greystone_Tough'"));

	if (asset.Succeeded())
	{
		skeletalMesh = asset.Object;
		GetMesh()->SetSkeletalMesh(skeletalMesh);
		GetMesh()->SetupAttachment(GetCapsuleComponent());
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

		BoxCollisionAttack = CreateDefaultSubobject<UBoxComponent>("Box Collision Attack");
		BoxCollisionAttack->SetupAttachment(RootComponent);
	}
}

void ACEnemy_Boss::BeginPlay()
{
	Super::BeginPlay();

	if (BoxCollisionAttack)
	{
		BoxCollisionAttack->OnComponentBeginOverlap.AddDynamic(this, &ACEnemy_Boss::OnBoxCollisionAttackBeginOverlap);
		BoxCollisionAttack->OnComponentEndOverlap.AddDynamic(this, &ACEnemy_Boss::OnBoxCollisionAttackEndOverlap);
	}

	//GetWorldTimerManager().SetTimer(OnAttackTimer, this, &ACEnemy_Boss::OnAttack, 1.0f, true);
}

void ACEnemy_Boss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/* �� �Լ��� ȣ���ϴ� ��ü�� ������ �� ���ε尡 ����ǵ��� �ؾߵȴ�. 
	 * �ƴϸ� Ensure Condition Failed �αװ� ��µȴ�.
	 */
	if (ensure(this))
	{
		if (GetOpponent() && CanBind)
		{
			CanBind = false;

			Cast<ACPlayer>(GetOpponent())->EnableBind();
		}
	}
}

void ACEnemy_Boss::OnAttack()
{
	Super::OnAttack();

	GetWorldTimerManager().PauseTimer(OnAttackTimer);

	if (GetDistanceTo(GetOpponent()) <= NormalAttackRange)
	{
		if (Hp / MaxHp <= 0.5f)
		{
			int32 selectS = UKismetMathLibrary::RandomIntegerInRange(0, 2);

			if (NormalSpecialAttackDatas[selectS].Montage)
				NormalSpecialAttackDatas[selectS].PlayMontage(this);

			return;
		}
		
		int32 select = UKismetMathLibrary::RandomIntegerInRange(0, 2);

		if (NormalAttackDatas[select].Montage)
			NormalAttackDatas[select].PlayMontage(this);
	}
	if(GetDistanceTo(GetOpponent()) > NormalAttackRange)
	{	
		if (Hp / MaxHp <= 0.5f)
		{
			if (!Once_IsBossFriendCalled)
			{
				SpawnRangeSpecialAttackEnemySpawnBossEffect();
				bActivateRotateToOpponent = false;

				if (RangeSpecialAttackDatas[3].Montage)
					RangeSpecialAttackDatas[3].PlayMontage(this);

				Once_IsBossFriendCalled = true;

				return;
			}
			
			int32 selectS = UKismetMathLibrary::RandomIntegerInRange(0, 2);

			//if (selectS == 0)
			//{	
			//	GetWorldTimerManager().SetTimer(FinishSearchTimer, FTimerDelegate::CreateLambda([&]()
			//	{
			//		GetWorldTimerManager().SetTimer(SearchTimer, FTimerDelegate::CreateLambda([&]()
			//		{
			//			auto controllingPawn = GetController()->GetPawn();
			//			UNavigationSystemV1* navSystem = UNavigationSystemV1::GetNavigationSystem(controllingPawn->GetWorld());
			//			FNavLocation nextLocation;
			//			FVector current = controllingPawn->GetActorLocation();
			//
			//			if (navSystem->GetRandomPointInNavigableRadius(current, UKismetMathLibrary::RandomFloatInRange(450.0f, 750.0f), nextLocation))
			//			{
			//				current = FVector(nextLocation.Location.X, nextLocation.Location.Y, nextLocation.Location.Z);
			//				SearchedLocations.Add(current);
			//			}
			//
			//		}), 0.1f, true);
			//	}), 3.0f, false);
			//	
			//	// 3�ʰ� ��ġ �� ã������ 
			//	if (GetWorldTimerManager().GetTimerElapsed(FinishSearchTimer) >= 3.0)
			//	{
			//		GetWorldTimerManager().ClearTimer(FinishSearchTimer);
			//		
			//		if (RangeSpecialAttackDatas[selectS].Montage)
			//			RangeSpecialAttackDatas[selectS].PlayMontage(this);
			//	}
			//	
			//	return;
			//}

			if (RangeSpecialAttackDatas[selectS].Montage)
				RangeSpecialAttackDatas[selectS].PlayMontage(this);

			return;
		}

		int32 select = UKismetMathLibrary::RandomIntegerInRange(0, 2);
		
		if (RangeAttackDatas[select].Montage)
			RangeAttackDatas[select].PlayMontage(this);
	}
}

void ACEnemy_Boss::OnMontageEnded(UAnimMontage* InMontage, bool InInterrupted)
{
	//GetWorldTimerManager().UnPauseTimer(OnAttackTimer);

	Super::OnMontageEnded(InMontage, InInterrupted);

	bActivateRotateToOpponent = true;
}

void ACEnemy_Boss::OnBoxCollisionAttackBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CheckNull(OtherActor);
	//CheckTrue(OverlappedComponent == GetCapsuleComponent());
	
	ICInterface_PlayerState* playerInterface = Cast<ICInterface_PlayerState>(OtherActor);

	if (playerInterface)
		playerInterface->SetPlayerIsAttackByBoss(true);
}

void ACEnemy_Boss::OnBoxCollisionAttackEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CheckNull(OtherActor);
	//CheckTrue(OverlappedComponent == GetCapsuleComponent());

	ICInterface_PlayerState* playerInterface = Cast<ICInterface_PlayerState>(OtherActor);

	if (playerInterface)
		playerInterface->SetPlayerIsAttackByBoss(false);
}

void ACEnemy_Boss::Notify_BossAttack(EBossAttackType InType)
{
	CurrentBossAttackType = InType;
	
	if (OnBossAttack.IsBound())
		OnBossAttack.Broadcast(CurrentBossAttackType);
}

void ACEnemy_Boss::Notify_SpawnBossRangeAttack1Projectile(TSubclassOf<ACProjectile> InProjectileClass)
{
	if (SearchedLocations.Num() <= 0) return;

	FTimerHandle spawnTimer;

	for (int i = 0; i < SearchedLocations.Num(); i++)
		ACProjectile::SpawnProjectile(this, InProjectileClass, FVector(SearchedLocations[i].X, SearchedLocations[i].Y, SearchedLocations[i].Z + 1000.0f));
}
