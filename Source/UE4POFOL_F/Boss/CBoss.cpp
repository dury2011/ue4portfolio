#include "Boss/CBoss.h"
#include "Global.h"
#include "GenericTeamAgentInterface.h"
#include "Component/CBossComponent.h"
#include "Boss/CDataAsset_Boss.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Weapon/CBall.h"
#include "Weapon/CRocket.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraSystem.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon/CRocket.h"
#include "Weapon/CWeapon.h"
#include "Math/TransformNonVectorized.h"
#include "Weapon/CDoAction.h"

#define LOG_CBOSS

ACBoss::ACBoss()
{
	PrimaryActorTick.bCanEverTick = true;


	USkeletalMesh* bossMesh;
	CHelpers::GetAsset<USkeletalMesh>(&bossMesh, "SkeletalMesh'/Game/ParagonSteel/Characters/Heroes/Steel/Meshes/Steel.Steel'");
	

	GetMesh()->SetSkeletalMesh(bossMesh);
	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

	CHelpers::GetClass<UAnimInstance>(&AnimInstance, "AnimBlueprint'/Game/FORUE4POFOL/Boss/blueprint/ABP_CBoss.ABP_CBoss_C'");
	GetMesh()->SetAnimInstanceClass(AnimInstance);

	CHelpers::CreateActorComponent<UCBossComponent>(this, &BossComponent, "CBossComponent");
	
	RootComponent = GetCapsuleComponent();
	
	CHelpers::GetClass<ACWeapon>(&GroundCrackWeaponClass, "Blueprint'/Game/FORUE4POFOL/Weapon/BP_CWeapon_EarthCrack.BP_CWeapon_EarthCrack_C'");
}

void ACBoss::BeginPlay()
{
	Super::BeginPlay();

	BossComponent->SetIdleMode();

	GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &ACBoss::MontageEnded);

	GetWorldTimerManager().SetTimer(Timer, this, &ACBoss::Attack, AttackDelaySecond, true);
}

void ACBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Blackboard)
		Opponent = Cast<ACharacter>(Blackboard->GetValueAsObject(FName("Player")));

	if (Opponent)
	{
		FVector directionToOpponent = Opponent->GetActorLocation() - GetActorLocation();
		directionToOpponent.Z = 0.0f;
		FRotator targetRotation = FRotationMatrix::MakeFromX(directionToOpponent).Rotator();
		//FRotator targetRotation = directionToOpponent.Rotation();

		SetActorRotation(FMath::RInterpTo(GetActorRotation(), targetRotation, GetWorld()->GetDeltaSeconds(), 2.8f));
	}

	if (BossComponent->IsAttackJumpMode())
		CheckLandingLocation();

#ifdef LOG_CBOSS
	if(Opponent)
		CLog::Print(Opponent->GetName(), 6);
#endif LOG_CBOSS
}

void ACBoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACBoss::CreateLineTrace()
{
	FVector lineTraceStart = GetActorLocation();
	FVector lineTraceEnd = lineTraceStart + FQuat(GetActorRotation()).GetUpVector() - 6000.0f;
	TArray<AActor*> lineTraceIgnoreActor;
	TEnumAsByte<EDrawDebugTrace::Type> drawDebugType;

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
		FColor(1,0,0,1),
		FLinearColor::Green,
		0.0f
	);
}

void ACBoss::CheckLandingLocation()
{
	CreateLineTrace();

	FHitResult hitResult = HitResult;;

	//if (hitResult.bBlockingHit)
	//	LandingNiagaraSpawnLoc = hitResult.Location;
}

void ACBoss::Attack()
{
	CheckNull(Opponent);
	
	GetWorldTimerManager().PauseTimer(Timer);
	
	BossComponent->DoAction();
}

void ACBoss::ShootRocket()
{
	CheckNull(Opponent);

	FActorSpawnParameters spawnParams;

	spawnParams.Owner = this;

	ACRocket* rocket = GetWorld()->SpawnActor<ACRocket>
		(
			RocketClass,
			GetMesh()->GetSocketLocation(FName("spawnRocket")),
			GetMesh()->GetSocketRotation(FName("spawnRocket")),
			spawnParams
		);
	
	rocket->GetProjectile()->HomingTargetComponent = Opponent->GetCapsuleComponent();
	rocket->GetProjectile()->bIsHomingProjectile = true;

	FVector shootDirection = GetActorUpVector();

	rocket->Shoot(shootDirection);
}
	
void ACBoss::ShootBall()
{
	CheckNull(Opponent);

	FActorSpawnParameters spawnParams;

	spawnParams.Owner = this;

	for (int32 i = 0; i < 8; i++)
	{
		FName socketName = "spawnBall";
		FString socketNametoString = socketName.ToString();
		socketNametoString.Append(FString::FromInt(i));
		FName socketNameI = FName(*socketNametoString);

		ACBall* ball = GetWorld()->SpawnActor<ACBall>
			(
				BallClass,
				GetMesh()->GetSocketLocation(FName(socketNameI)),
				GetMesh()->GetSocketRotation(FName(socketNameI)),
				spawnParams
			);

		FVector shootDirection = (GetMesh()->GetSocketLocation(FName(socketNameI)) - GetActorLocation()) * FVector(1.0f, 1.0f, -10.0f);
		ball->Shoot(shootDirection);
	}
	
	for (int32 i = 0; i < 4; i++)
	{
		FName socketName = "spawnEarthCrack";
		FString socketNametoString = socketName.ToString();
		socketNametoString.Append(FString::FromInt(i));
		FName socketNameI = FName(*socketNametoString);
		
		FTransform targetTransform = GetMesh()->GetSocketTransform(FName(socketNameI));

		targetTransform.SetLocation(GetMesh()->GetSocketLocation(FName(socketNameI)));
		targetTransform.SetRotation(FQuat(GetMesh()->GetSocketRotation(FName(socketNameI))));

		ACWeapon* spawnedWeapon = GetWorld()->SpawnActorDeferred<ACWeapon>(GroundCrackWeaponClass, targetTransform, this);

		spawnedWeapon->SetOwner(this);

		spawnedWeapon->OnWeaponBeginOverlap.AddDynamic(BossComponent->GetDataAsset()->GetDoAction(), &UCDoAction::OnWeaponBeginOverlap);
		spawnedWeapon->OnWeaponEndOverlap.AddDynamic(BossComponent->GetDataAsset()->GetDoAction(), &UCDoAction::OnWeaponEndOverlap);

		UGameplayStatics::FinishSpawningActor(spawnedWeapon, targetTransform);
	}
}

void ACBoss::SpawnFriend()
{
	CheckNull(Opponent);

	FActorSpawnParameters spawnParams;

	spawnParams.Owner = this;

	for (int32 i = 0; i < 4; i++)
	{
		FName socketName = "spawnFriend";
		FString socketNametoString = socketName.ToString();
		socketNametoString.Append(FString::FromInt(i));
		FName socketNameI = FName(*socketNametoString);

		Enemy[i] = GetWorld()->SpawnActor<ACEnemy>
			(
				CallFriendClass,
				GetMesh()->GetSocketLocation(socketNameI),
				GetMesh()->GetSocketRotation(socketNameI),
				spawnParams
			);
	}
}

void ACBoss::SpawnNiagaraSystem()
{
	if (LocationMarkEmitter)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation
		(
			GetWorld(),
			LocationMarkEmitter,
			EmitterLocation
		);
	}
}


void ACBoss::MoveOpponentLoc()
{
	CheckNull(Opponent);

	FLatentActionInfo latentInfo;
	latentInfo.CallbackTarget = this;

	UKismetSystemLibrary::MoveComponentTo
	(
		GetCapsuleComponent(),
		Opponent->GetMesh()->GetSocketLocation("bossAttack"),
		GetActorRotation(),
		false,
		true,
		1.0f,
		false,
		EMoveComponentAction::Move,
		latentInfo
	);
}

void ACBoss::OwnedWeaponBeginOverlap(class ACharacter* InAttacker, class AActor* InAttackCauser, class ACharacter* InOtherCharacter)
{

}

void ACBoss::OwnedWeaponEndOverlap(class ACharacter* InAttacker, class AActor* InAttackCauser, class ACharacter* InOtherCharacter)
{

}

void ACBoss::OwnedWeaponOnCollision()
{

}

void ACBoss::OwnedWeaponOffCollision()
{

}

void ACBoss::MontageEnded(UAnimMontage* InMontage, bool InInterrupted)
{
	GetWorldTimerManager().UnPauseTimer(Timer);

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_NavWalking);
}

