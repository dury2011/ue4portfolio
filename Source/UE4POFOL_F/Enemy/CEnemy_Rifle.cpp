/* MEMO: ACEnemy_Rifle.cpp 코드 배치 정리 완료  */
#include "Enemy/CEnemy_Rifle.h"
#include "Global.h"
#include "Components/CapsuleComponent.h"

ACEnemy_Rifle::ACEnemy_Rifle()
{
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = GetCapsuleComponent();

	USkeletalMesh* skeletalMesh;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> asset(*FString("SkeletalMesh'/Game/ParagonMinions/Characters/Minions/Dusk_Minions/Meshes/Minion_Lane_Ranged_Core_Dusk.Minion_Lane_Ranged_Core_Dusk'"));
	
	if (asset.Succeeded())
	{
		skeletalMesh = asset.Object;
		GetMesh()->SetSkeletalMesh(skeletalMesh);
	}

	ConstructorHelpers::FClassFinder<UAnimInstance> animInstance(*FString("AnimBlueprint'/Game/FORUE4POFOL/Enemy/Blueprint/ABP_CEnemy_Rifle.ABP_CEnemy_Rifle_C'"));

	if (animInstance.Succeeded())
	{
		AnimInstance = animInstance.Class;
	}

	GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	GetMesh()->SetAnimInstanceClass(AnimInstance);
}

void ACEnemy_Rifle::BeginPlay()
{
	Super::BeginPlay();

	GetComponents<UCapsuleComponent>(CapsuleCollisions);

	for (UCapsuleComponent* capsule : CapsuleCollisions)
	{
		capsule->OnComponentBeginOverlap.AddDynamic(this, &ACEnemy_Rifle::OnBeginOverlap);
		capsule->OnComponentEndOverlap.AddDynamic(this, &ACEnemy_Rifle::OnEndOverlap);
		capsule->OnComponentHit.AddDynamic(this, &ACEnemy_Rifle::OnHit);
	}

	GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &ACEnemy_Rifle::OnMontageEnded);
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

}

void ACEnemy_Rifle::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void ACEnemy_Rifle::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ACEnemy_Rifle::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

}

void ACEnemy_Rifle::OnMontageEnded(UAnimMontage* InMontage, bool InInterrupted)
{

}


