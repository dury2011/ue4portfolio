/* MEMO: CEnemy.cpp 코드 배치 정리 완료 */
#include "Enemy/CEnemy.h"
#include "Global.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimInstance.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Weapon/CWeaponStructure.h"
#include "Animation/AnimMontage.h"
#include "Blueprint/UserWidget.h"
#include "Player/CPlayer.h"
#include "AI/CAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/CRifle.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Component/CCharacterComponent.h"

//#define DEBUG_CENEMY

ACEnemy::ACEnemy()
	:HealthBarDisplayTime(2.f)
{
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = GetCapsuleComponent();
}

void ACEnemy::BeginPlay()
{
	Super::BeginPlay(); 

	GetComponents<UCapsuleComponent>(CapsuleCollisions);

	for (UShapeComponent* collision : CapsuleCollisions)
	{
		collision->OnComponentBeginOverlap.AddDynamic(this, &ACEnemy::OnBeginOverlap);
		collision->OnComponentEndOverlap.AddDynamic(this, &ACEnemy::OnEndOverlap);
		collision->OnComponentHit.AddDynamic(this, &ACEnemy::OnHit);
	}

	if (GetMesh()->GetAnimInstance())
		GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &ACEnemy::OnMontageEnded);
	
	OnStateTypeChange(EEnemyStateType::Idle);
}

void ACEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(Blackboard)
		Opponent = Cast<ACharacter>(Blackboard->GetValueAsObject(FName("Player")));
	
	if (Opponent)
	{
		FVector directionToOpponent = Opponent->GetActorLocation() - GetActorLocation();
		directionToOpponent.Z = 0.0f;
		FRotator targetRotation = FRotationMatrix::MakeFromX(directionToOpponent).Rotator();

		SetActorRotation(FMath::RInterpTo(GetActorRotation(), targetRotation, GetWorld()->GetDeltaSeconds(), 5.0f));
	}

	UpdateHitNumbers();

	//
	//if (Controller)
	//{
	//	//UObject* obj = Controller->GetBlackboardComponent()->GetValueAsObject(FName("Player"));
	//	//Player = Cast<ACPlayer>(obj);
	//
	//	float yawDeg = FMath::DegreesToRadians(GetActorRotation().Yaw);
	//	float actorX = GetActorLocation().X;
	//	float actorY = GetActorLocation().Y;
	//	float x1 = GetActorLocation().X;
	//	float x2 = GetActorLocation().X;
	//	float y1 = GetActorLocation().Y + 100.f;
	//	float y2 = GetActorLocation().Y - 100.f;
	//
	//	FMatrix2x2 rotationMatrix2D(FMath::Cos(yawDeg), -(FMath::Sin(yawDeg)), FMath::Sin(yawDeg), FMath::Cos(yawDeg));
	//
	//	float cos00, mSin01, sin10, cos11;
	//
	//	rotationMatrix2D.GetMatrix(cos00, mSin01, sin10, cos11);
	//
	//	// rotationMatrix2D * (x1, y1)
	//	float newX1 = (cos00 * (x1 - actorX)) + (mSin01 * (y1 - actorY));
	//	float newY1 = (sin10 * (x1 - actorX)) + (cos11 * (y1 - actorY));
	//
	//	// rotationMatrix2D * (x2, y2)
	//	float newX2 = (cos00 * (x2 - actorX)) + (mSin01 * (y2 - actorY));
	//	float newY2 = (sin10 * (x2 - actorX)) + (cos11 * (y2 - actorY));
	//	
	//	if (WeaponComponent->IsRifleMode() || WeaponComponent->IsOnehandMode())
	//	{	
	//		WorldAttackPoint = FVector(Pos.X + newX1, Pos.Y + newY1, Pos.Z);
	//		WorldAttackPoint2 = FVector(Pos.X + newX2, Pos.Y + newY2, Pos.Z);
	//	
	//		Controller->GetBlackboardComponent()->SetValueAsVector(TEXT("AttackPoint"), WorldAttackPoint);
	//		Controller->GetBlackboardComponent()->SetValueAsVector(TEXT("AttackPoint2"), WorldAttackPoint2);
	//	}
	//	else
	//	{
	//		WorldAttackPoint = FVector(GetActorLocation().X + newX1, GetActorLocation().Y + newY1, GetActorLocation().Z);
	//		WorldAttackPoint2 = FVector(GetActorLocation().X + newX2, GetActorLocation().Y + newY2, GetActorLocation().Z);
	//	}
	//}

#ifdef DEBUG_CENEMY
	DrawDebugSphere(GetWorld(), WorldAttackPoint, 25.f, 12, FColor::Red, false);
	DrawDebugSphere(GetWorld(), WorldAttackPoint2, 25.f, 12, FColor::Red, false);
#endif DEBUG_CENEMY
}

float ACEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Damaged.DamageAmount = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Damaged.DamageEvent = (FActionDamageEvent*)&DamageEvent; // EXPLAIN: 여기서 TakeDamage할 Character의 HitData가 할당됨
	Damaged.EventInstigator = EventInstigator;
	Damaged.DamageCauser = DamageCauser;

	//TODO: 이렇게 해도 될 지 모르겠다.
	bDamage = true;

	ShakeCamera(Damaged);
	ShowHitNumber(DamageAmount, this->GetActorLocation());

	// BUG: 컴포넌트 생성 후 리턴 값 수정해야된다.
	return 0.0f;
}

void ACEnemy::DoAction()
{

}

void ACEnemy::Damage()
{
	ShowHealthBar();
	
	Damaged.DamageAmount = 0.0f;
	
	FVector start = GetActorLocation();
	FVector target = Damaged.EventInstigator->GetPawn()->GetActorLocation();
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(start, target));
	
	if (!!Damaged.DamageEvent)
	{
		FDamageData* DamageData = Damaged.DamageEvent->DamageData;

		//if (StateComponent->IsAttackSkillMode())
		//{
		//	hitData->PlayMontage(this);
		//	hitData->PlayEffect(GetWorld(), this);
		//	hitData->PlaySoundCue(GetWorld(), GetActorLocation());
		//
		//	return;
		//}
		
		FVector direction = target - start;
		direction.Normalize();
		
		FTransform transform;
		transform.SetLocation(GetActorLocation());
		
		LaunchCharacter(-direction * DamageData->Launch, true, false);

		DamageData->PlayMontage(this);
		DamageData->PlayEffect(GetWorld(), this);
		DamageData->PlayHitStop(GetWorld());
		DamageData->PlaySoundCue(GetWorld(), GetActorLocation());

		bDamage = false;
	}

}

void ACEnemy::Dead()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACEnemy::ShakeCamera(FDamaged damage)
{
	APlayerController* controller = Cast<APlayerController>(damage.EventInstigator);

	controller->PlayerCameraManager->StartCameraShake(CameraShakeClass);
}

void ACEnemy::StoreHitNumber(UUserWidget* InHitNumber, FVector InLocation)
{
	HitNumbers.Add(InHitNumber, InLocation);

	FTimerHandle HitNumberTimer;
	FTimerDelegate HitNumberDelegate;
	HitNumberDelegate.BindUFunction(this, FName("DestroyHitNumber"), InHitNumber);
	GetWorld()->GetTimerManager().SetTimer(HitNumberTimer, HitNumberDelegate, HitNumberDestroyTime, false);
}

void ACEnemy::DestroyHitNumber(UUserWidget* InHitNumber)
{
	HitNumbers.Remove(InHitNumber);
	InHitNumber->RemoveFromParent(); // 뷰포트에서 위젯을 지워줄 것이다.
}

void ACEnemy::UpdateHitNumbers()
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

void ACEnemy::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(HealthBarTimer, this, &ACEnemy::HideHealthBar, HealthBarDisplayTime);
}

void ACEnemy::OnStateTypeChange(EEnemyStateType InCurrentStateType)
{
	EEnemyStateType PreviousStateType = CurrentStateType;
	CurrentStateType = InCurrentStateType;

	if (OnEnemyStateTypeChanged.IsBound())
		OnEnemyStateTypeChanged.Broadcast(PreviousStateType, CurrentStateType);
}

void ACEnemy::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void ACEnemy::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ACEnemy::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}

void ACEnemy::OnMontageEnded(UAnimMontage* InMontage, bool InInterrupted)
{
	CharacterComponent->SetIsMontagePlaying(false);
}
