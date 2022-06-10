/* MEMO: ACEnemy_Rifle.cpp 코드 배치 정리 완료  */
#include "Enemy/CEnemy_Rifle.h"
#include "Global.h"
#include "Components/CapsuleComponent.h"
#include "Component/CCharacterComponent.h"

ACEnemy_Rifle::ACEnemy_Rifle()
{
	PrimaryActorTick.bCanEverTick = true;
	
	USkeletalMesh* skeletalMesh;
	ConstructorHelpers::FObjectFinder<USkeletalMesh> asset(*FString("SkeletalMesh'/Game/ParagonMinions/Characters/Minions/Dusk_Minions/Meshes/Minion_Lane_Ranged_Core_Dusk.Minion_Lane_Ranged_Core_Dusk'"));

	if (asset.Succeeded())
	{
		skeletalMesh = asset.Object;
		GetMesh()->SetSkeletalMesh(skeletalMesh);
		GetMesh()->SetupAttachment(GetCapsuleComponent());
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}

	ConstructorHelpers::FClassFinder<UAnimInstance> animInstance(*FString("AnimBlueprint'/Game/FORUE4POFOL/Enemy/Blueprint/ABP_CEnemy_Rifle.ABP_CEnemy_Rifle_C'"));

	if (animInstance.Succeeded())
	{
		AnimInstance = animInstance.Class;
		GetMesh()->SetAnimInstanceClass(AnimInstance);
	}

	CharacterComponent = CreateDefaultSubobject<UCCharacterComponent>("CharacterComponent");
	CharacterComponent->SetCurrentStateType(EStateType::Idle);
}

void ACEnemy_Rifle::BeginPlay()
{
	Super::BeginPlay();

} 

void ACEnemy_Rifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACEnemy_Rifle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACEnemy_Rifle::OnFire()
{
	CharacterComponent->SetCurrentWeaponType(EWeaponType::Spell);
	CharacterComponent->SetCurrentStateType(EStateType::Attack);
	
	CharacterComponent->SetIsMontagePlaying(true);
	CharacterComponent->GetActionDatasSpell(0).PlayMontage(this);
}

void ACEnemy_Rifle::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	
	CharacterComponent->SetCurrentStateType(EStateType::Damage);
}

void ACEnemy_Rifle::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void ACEnemy_Rifle::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
	
	CharacterComponent->SetCurrentStateType(EStateType::Damage);
}



