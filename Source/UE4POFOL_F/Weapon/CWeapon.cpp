/* MEMO: CWeapon.cpp 코드 배치 정리 완료 */                                                                                                                                                                                                                                             #include "Weapon/CWeapon.h"
#include "Weapon/CWeapon.h"
#include "Global.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Engine.h"
//#include "Component/CStateComponent.h"
#include "Enemy/CEnemy.h"
#include "GenericTeamAgentInterface.h"
#include "Component/CCharacterComponent.h"

ACWeapon::ACWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateComponent<USceneComponent>(this, &Root, "Root");

	SetRootComponent(Root);
}

void ACWeapon::BeginPlay()
{
	if (GetOwner())
	{
		OwnerCharacter = Cast<ACharacter>(GetOwner());
		//StateComponent = CHelpers::GetComponent<UCStateComponent>(OwnerCharacter);
	}

	GetComponents<UShapeComponent>(Collisions);
	
	OffCollision(); // 처음 시작시에는 Weapon의 Collision은 Off 되어야함. 

	for (UShapeComponent* collision : Collisions)
	{
		collision->OnComponentBeginOverlap.AddDynamic(this, &ACWeapon::OnBeginOverlap);
		collision->OnComponentEndOverlap.AddDynamic(this, &ACWeapon::OnEndOverlap);
		collision->OnComponentHit.AddDynamic(this, &ACWeapon::OnHit);	
	}
	
	Super::BeginPlay();
}

void ACWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

ACWeapon* ACWeapon::SpawnWeapon(ACharacter* InSpawner, TSubclassOf<ACWeapon> InWeaponClass, FVector InSpawnLocation)
{
	FActorSpawnParameters params;
	params.Owner = InSpawner;

	return InSpawner->GetWorld()->SpawnActor<ACWeapon>
		(
			InWeaponClass,
			InSpawnLocation,
			FRotator::ZeroRotator,
			params
		);
}

void ACWeapon::DestroyWeapon_Implementation()
{
	Destroy();
}

void ACWeapon::AttachTo(FName InSocketName)
{
	AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), InSocketName);
}

void ACWeapon::AttachCollisionTo(FName InSocketName)
{
	for (UShapeComponent* component : Collisions)
	{
		if (component->GetName() == InSocketName.ToString())
		{
			component->AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true), InSocketName);

			return;
		}
	}
}

void ACWeapon::OnCollision()
{
	for (UShapeComponent* collision : Collisions)
		collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); 
	
	//if (StateComponent->IsActionMode())
	//{
	//	if (OnWeaponCollision.IsBound())
	//		OnWeaponCollision.Broadcast();
	//}
}

void ACWeapon::OffCollision()
{
	for (UShapeComponent* collision : Collisions)
		collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	//if (StateComponent->IsActionMode())
	//{
	//	if (OffWeaponCollision.IsBound())
	//		OffWeaponCollision.Broadcast();
	//}
}

/* TODO: Delegate를 이용하여 전달하는 것은 너무 많이 사용하면 개인적으로 코드의 독립성과 응집성이 떨어진다고 생각한다. 
 * WeaponBeginOverlap과 WeaponEndOverlap은 CWeapon 클래스의 고유 기능이여야만한다. */
void ACWeapon::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CheckNull(OverlappedComponent);
	CheckNull(OtherActor);
	CheckNull(OtherComp);
	
	CheckTrue(OwnerCharacter == OtherActor);
	CheckTrue(OwnerCharacter->GetCapsuleComponent() == OverlappedComponent);
	CheckTrue(OwnerCharacter->GetClass() == OtherActor->GetClass());
	CheckTrue(IsOverlapped);
	
	GLog->Log("ACWeapon::OnBeginOverlap");
	//UCCharacterComponent* OverlappedActorCharacterComponent = Cast<UCCharacterComponent>(OtherActor->GetComponentByClass(UCCharacterComponent::StaticClass()));
	//
	//if (OverlappedActorCharacterComponent)
	//{
		//OverlappedActorCharacterComponent->GetDamageData(0).PlayMontage(Cast<ACharacter>(OtherActor));
		// BUG: Apply Damage 여기 확인하기 
	//}
	IsOverlapped = true;

	float percentage = UKismetMathLibrary::RandomFloatInRange(0.0f, 100.0f);
	float randomDeviation = UKismetMathLibrary::RandomIntegerInRange(0,RandomDeviation);
	int32 pureDamage = PureDamage;
	int32 criticalDamage = CriticalDamage;
	int32 applyDamage = ApplyDamage;
	
	if ((percentage / 100) <= CriticalPercentage)
	{
		pureDamage += CriticalDamage;
		applyDamage = pureDamage + randomDeviation;
	}
	else
		applyDamage = pureDamage + randomDeviation;
	
	if (OwnerCharacter && (OwnerCharacter->GetController() == GetWorld()->GetFirstPlayerController()))
	{
		OwnerCharacter->CustomTimeDilation = 10e-5f;

		GetWorldTimerManager().SetTimer(StopTimer, this, &ACWeapon::RecoverDilation, StopTime, true);
	}

	/* TODO: 2번 째 인수 float BaseDamage는 0.0f 이하이면 함수 몸체에서 도입에서 if 문으로 걸러져서 몸체 전체가 작동하지 않음 */
	UGameplayStatics::ApplyDamage(OtherActor, applyDamage, OwnerCharacter->GetController(), this, NULL);

	//GLog->Log("ACWeapon::WeaponBeginOverlap()");
	
	//if (OtherActor->ActorHasTag(FName("Enemy_BossFriend")))
	//	return;
	//
	//FGenericTeamId ownerId = Cast<ACEnemy>(OwnerCharacter)->GetGenericTeamId();
	//FGenericTeamId otherId = Cast<ACEnemy>(OtherActor)->GetGenericTeamId();
	//
	//CheckTrue(ownerId == otherId);
	//
	// CheckTrue(OwnerCharacter->GetClass() == OtherActor->GetClass());
	//CheckTrue(OwnerCharacter->GetCapsuleComponent() == OverlappedComponent);
	//
	// MEMO: 보스는 StateComponent가 없다.  
	//if (!StateComponent)
		//if (OnWeaponBeginOverlap.IsBound()) 
		//{
			//OnWeaponBeginOverlap.Broadcast(OwnerCharacter, this, Cast<ACharacter>(OtherActor));
	//
			//return;
		//}
	//
	// BUG: 작동은 되나 매우 불안정한 코드, 반드시 전면 수정 필요
	//if (StateComponent->IsAttackNormalMode())
	//{
		//if (OnWeaponBeginOverlap.IsBound())
			//OnWeaponBeginOverlap.Broadcast(OwnerCharacter, this, Cast<ACharacter>(OtherActor)); 
	//}
	//
	//if (StateComponent->IsAttackCriticalMode())
	//{
	//	if (OnWeaponBeginOverlapForCritical.IsBound())
	//		OnWeaponBeginOverlapForCritical.Broadcast(OwnerCharacter, this, Cast<ACharacter>(OtherActor));
	//}
	//
	//if (StateComponent->IsAttackSkillMode())
	//{
	//	if(OnWeaponBeginOverlapForSkill.IsBound())
	//		OnWeaponBeginOverlapForSkill.Broadcast(OwnerCharacter, this, Cast<ACharacter>(OtherActor));
	//}
} 

void ACWeapon::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CheckNull(OverlappedComponent);
	CheckNull(OtherActor);
	CheckNull(OtherComp);

	CheckTrue(OwnerCharacter == OtherActor);
	CheckTrue(OwnerCharacter->GetCapsuleComponent() == OverlappedComponent);
	CheckTrue(OwnerCharacter->GetClass() == OtherActor->GetClass());
	CheckFalse(IsOverlapped);
	if (OtherActor->ActorHasTag(FName("Enemy_BossFriend")))
		return;
	
	IsOverlapped = false;

	GLog->Log("ACWeapon::OnEndOverlap()");

	//// MEMO: 보스는 StateComponent가 없다.
	//if (!StateComponent)
	//	if (OnWeaponEndOverlap.IsBound())
	//	{
	//		OnWeaponEndOverlap.Broadcast(OwnerCharacter, this, Cast<ACharacter>(OtherActor));
	//
	//		return;
	//	}
	//
	//// BUG: 작동은 되나 매우 불안정한 코드, 반드시 전면 수정 필요
	//if (StateComponent->IsAttackNormalMode())
	//{
	//	if (OnWeaponEndOverlap.IsBound())
	//		OnWeaponEndOverlap.Broadcast(OwnerCharacter, this, Cast<ACharacter>(OtherActor));
	//}
	//
	//if (StateComponent->IsAttackCriticalMode())
	//{
	//	if (OnWeaponEndOverlapForCritical.IsBound())
	//		OnWeaponEndOverlapForCritical.Broadcast(OwnerCharacter, this, Cast<ACharacter>(OtherActor));
	//}
	//
	//if (StateComponent->IsAttackSkillMode())
	//{
	//	if (OnWeaponEndOverlapForSkill.IsBound())
	//		OnWeaponEndOverlapForSkill.Broadcast(OwnerCharacter, this, Cast<ACharacter>(OtherActor));
	//}
}

void ACWeapon::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	CheckNull(HitComponent);
	CheckNull(OtherActor);
	CheckNull(OtherComp);
	
	
	//DestroyWeapon();

	//if (OnWeaponHit.IsBound())
	//	OnWeaponHit.Broadcast(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}

void ACWeapon::RecoverDilation()
{
	if (OwnerCharacter)
	{
		OwnerCharacter->CustomTimeDilation = 1.0f;

		GetWorldTimerManager().ClearTimer(StopTimer);
	}
}
