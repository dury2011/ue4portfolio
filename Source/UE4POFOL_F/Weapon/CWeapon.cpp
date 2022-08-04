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
#include "Interface/CInterface_PlayerState.h"
#include "Player/CPlayer.h"

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
	
	OffCollision(); // ó�� ���۽ÿ��� Weapon�� Collision�� Off �Ǿ����. 

	for (UShapeComponent* collision : Collisions)
	{
		collision->OnComponentBeginOverlap.AddDynamic(this, &ACWeapon::OnBeginOverlap);
		collision->OnComponentEndOverlap.AddDynamic(this, &ACWeapon::OnEndOverlap);
		collision->OnComponentHit.AddDynamic(this, &ACWeapon::OnHit);	
	}
	
	//Spawned = true;
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
	CheckTrue(IsOverlapped);

	// Shield�� BeginOverlap�� ��ü�� Enemy��
	if (!IsShield)
	{
		ICInterface_PlayerState* playerInterface = Cast<ICInterface_PlayerState>(OtherActor);

		if (playerInterface && playerInterface->GetPlayerUsingShield())
		{
			ACEnemy* enemy = Cast<ACEnemy>(OwnerCharacter);

			if (enemy && enemy->GetIsAttackByPlayer())
			{
				enemy->BlockedByShield();

				ACPlayer* player = Cast<ACPlayer>(OtherActor);

				if (player)
					player->ShieldDefencing(enemy);
				
				return;
			}
		}
	}

	if (IsSkillWeapon)
	{
		if (OtherActor)
		{
			ACEnemy* enemy = Cast<ACEnemy>(OtherActor);

			if (enemy)
			{
				enemy->SetIsAttackBySkillWeapon(true);
				enemy->TakeDamage_OpponentUsingSkillWeapon();
				
				return;
			}
		}
	}
	
	GLog->Log("ACWeapon::OnBeginOverlap_SkillWeapon");

	IsOverlapped = true;

	float percentage = UKismetMathLibrary::RandomFloatInRange(0.0f, 100.0f);
	float randomDeviation = UKismetMathLibrary::RandomIntegerInRange(0,RandomDeviation);
	int32 pureDamage = PureDamage;
	int32 criticalDamage = CriticalDamage;
	
	/* MEMO: Skill Weapon�� Enemy�� Begin Overlap�� ���
	////* enemy���� Skill Weapon�� �ݸ��� ��ٰ� �˷��ֱ�
	////* enemy���� ������ �Լ� ȣ��ǵ��� �ϱ� */
	//if (OtherActor->ActorHasTag("Enemy"))
	//{
	//	ACEnemy* enemy = Cast<ACEnemy>(OtherActor);
	//
	//	if (enemy)
	//	{
	//		/* MEMO: Notify���� Weapon�� Class�� �����ϸ� �ȵǴ� ����! 
	//		 * enemy���� ���� Skill Weapon�� Spawn�Ͽ� �� ��ü�� ���� ���ε带 �ؾߵ� 
	//		 */
	//		enemy->SetIsAttackBySkillWeapon(true);
	//		enemy->CallDelegateBind();
	//
	//		// MEMO: OnBeginOverlap�� ������ �� ���� �� ���� ȣ��ǹǷ� �̷��� Broadcast �ϸ�ȴ�.
	//		if (OnSkillWeaponBeginOverlap.IsBound())
	//			OnSkillWeaponBeginOverlap.Broadcast();
	//
	//		//if(enemy->GetIsAttackBySkillWeapon())
	//	}
	//}

	int32 applyDamage = ApplyDamage;
	
	if ((percentage / 100) <= CriticalPercentage)
	{
		pureDamage += CriticalDamage;
		applyDamage = pureDamage + randomDeviation;
	}
	else
		applyDamage = pureDamage + randomDeviation;
	
	UGameplayStatics::ApplyDamage(OtherActor, applyDamage, OwnerCharacter->GetController(), this, NULL);
	
	//if (OwnerCharacter && (OwnerCharacter->GetController() == GetWorld()->GetFirstPlayerController()))
	//{
	//	OwnerCharacter->CustomTimeDilation = 10e-5f;
	//
	//	GetWorldTimerManager().SetTimer(StopTimer, this, &ACWeapon::RecoverDilation, StopTime, true);
	//}
	//
	/* TODO: 2�� ° �μ� float BaseDamage�� 0.0f �����̸� �Լ� ��ü���� ���Կ��� if ������ �ɷ����� ��ü ��ü�� �۵����� ���� */
	//
	//GLog->Log("ACWeapon::WeaponBeginOverlap()");
	//
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
	CheckFalse(IsOverlapped);
	if (OtherActor->ActorHasTag(FName("Enemy_BossFriend")))
		return;
	
	IsOverlapped = false;

	if (IsSkillWeapon)
	{
		if (OtherActor)
		{
			ACEnemy* enemy = Cast<ACEnemy>(OtherActor);

			enemy->SetIsAttackBySkillWeapon(false);

			GLog->Log("ACWeapon::OnEndOverlap_SkillWeapon()");

			return;
		}
	}

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
	CheckTrue(OwnerCharacter == OtherActor);
	CheckTrue(OwnerCharacter->GetCapsuleComponent() == HitComponent);



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

//void ACWeapon::Destroyed()
//{
//	if (Enemy)
//		Enemy->SetIsAttackBySkillWeapon(false);
//	
//	Super::Destroyed();
//}
