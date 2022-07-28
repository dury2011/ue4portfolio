#include "Enemy/CEnemy_Boss.h"
#include "Global.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Interface/CInterface_PlayerState.h"
#include "Player/CPlayer.h"

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

	/* 이 함수를 호출하는 객체가 보증된 후 바인드가 실행되도록 해야된다. 
	 * 아니면 Ensure Condition Failed 로그가 출력된다.
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

	//GetWorldTimerManager().PauseTimer(OnAttackTimer);

	if (GetDistanceTo(GetOpponent()) <= NormalAttackRange)
	{
		int32 select = UKismetMathLibrary::RandomIntegerInRange(0, 4);

		if (ActionDatas[select].Montage)
			ActionDatas[select].PlayMontage(this);
	}
	if(GetDistanceTo(GetOpponent()) > NormalAttackRange)
	{
		
		int32 select = UKismetMathLibrary::RandomIntegerInRange(5, 8);
		
		if (select == 6)
			bActivateRotateToOpponent = false;

		if (ActionDatas[7].Montage)
			ActionDatas[7].PlayMontage(this);
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
