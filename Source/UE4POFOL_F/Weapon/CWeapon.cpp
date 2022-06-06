/* MEMO: CWeapon.cpp �ڵ� ��ġ ���� �Ϸ� */                                                                                                                                                                                                                                             #include "Weapon/CWeapon.h"
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
	: Damage (20.0f)
{
	PrimaryActorTick.bCanEverTick = true;
	CHelpers::CreateComponent<USceneComponent>(this, &Root, "Root");
	TSubclassOf<AActor> bullet;
}

void ACWeapon::BeginPlay()
{
	OffCollision(); // ó�� ���۽ÿ��� Weapon�� Collision�� Off �Ǿ����. 
	
	if (GetOwner())
	{
		OwnerCharacter = Cast<ACharacter>(GetOwner());
		//StateComponent = CHelpers::GetComponent<UCStateComponent>(OwnerCharacter);
	}

	GetComponents<UShapeComponent>(Collisions);

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

/* TODO: Delegate�� �̿��Ͽ� �����ϴ� ���� �ʹ� ���� ����ϸ� ���������� �ڵ��� �������� �������� �������ٰ� �����Ѵ�. 
 * WeaponBeginOverlap�� WeaponEndOverlap�� CWeapon Ŭ������ ���� ����̿��߸��Ѵ�. */
void ACWeapon::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CheckNull(OverlappedComponent);
	CheckNull(OtherActor);
	CheckNull(OtherComp);
	
	CheckTrue(OwnerCharacter == OtherActor);
	CheckTrue(OwnerCharacter->GetCapsuleComponent() == OverlappedComponent);
	CheckTrue(OwnerCharacter->GetClass() == OtherActor->GetClass());
	
	//UCCharacterComponent* OverlappedActorCharacterComponent = Cast<UCCharacterComponent>(OtherActor->GetComponentByClass(UCCharacterComponent::StaticClass()));
	//
	//if (OverlappedActorCharacterComponent)
	//{
		//OverlappedActorCharacterComponent->GetDamageData(0).PlayMontage(Cast<ACharacter>(OtherActor));
		// BUG: Apply Damage ���� Ȯ���ϱ� 
	//}

	// TODO: SendDamage, TakeDamage �����ϱ�
	UGameplayStatics::ApplyDamage(OtherActor, 10.0f, OwnerCharacter->GetController(), this, NULL);

	//DestroyWeapon();

	GLog->Log("ACWeapon::WeaponBeginOverlap()");
	
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
	// MEMO: ������ StateComponent�� ����.  
	//if (!StateComponent)
		//if (OnWeaponBeginOverlap.IsBound()) 
		//{
			//OnWeaponBeginOverlap.Broadcast(OwnerCharacter, this, Cast<ACharacter>(OtherActor));
	//
			//return;
		//}
	//
	// BUG: �۵��� �ǳ� �ſ� �Ҿ����� �ڵ�, �ݵ�� ���� ���� �ʿ�
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

	if (OtherActor->ActorHasTag(FName("Enemy_BossFriend")))
		return;
	 
	GLog->Log("ACWeapon::OnEndOverlap()");

	//// MEMO: ������ StateComponent�� ����.
	//if (!StateComponent)
	//	if (OnWeaponEndOverlap.IsBound())
	//	{
	//		OnWeaponEndOverlap.Broadcast(OwnerCharacter, this, Cast<ACharacter>(OtherActor));
	//
	//		return;
	//	}
	//
	//// BUG: �۵��� �ǳ� �ſ� �Ҿ����� �ڵ�, �ݵ�� ���� ���� �ʿ�
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

