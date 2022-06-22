#include "Enemy/CEnemy_Meele.h"
#include "Global.h"
#include "Components/CapsuleComponent.h"
#include "Component/CCharacterComponent.h"

ACEnemy_Meele::ACEnemy_Meele()
{
	USkeletalMesh* skeletalMesh;
	ConstructorHelpers::FObjectFinder<USkeletalMesh> asset(*FString("SkeletalMesh'/Game/ParagonMinions/Characters/Minions/White_Camp_Minion/Meshes/Minion_melee.Minion_melee'"));
	
	if (asset.Succeeded())
	{
		skeletalMesh = asset.Object;
		GetMesh()->SetSkeletalMesh(skeletalMesh);
		GetMesh()->SetupAttachment(GetCapsuleComponent());
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}

	ConstructorHelpers::FClassFinder<UAnimInstance> animInstance(*FString("AnimBlueprint'/Game/FORUE4POFOL/Enemy/Enemy_Meele/Blueprint/ABP_Enemy_Melee.ABP_Enemy_Melee_C'"));

	if (animInstance.Succeeded())
	{
		AnimInstance = animInstance.Class;
		GetMesh()->SetAnimInstanceClass(AnimInstance);
	}

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	CharacterComponent = CreateDefaultSubobject<UCCharacterComponent>("Character Component");
}

void ACEnemy_Meele::BeginPlay()
{
	Super::BeginPlay();
}

void ACEnemy_Meele::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACEnemy_Meele::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACEnemy_Meele::OnAttack()
{
	CheckTrue(bDamage);

	CharacterComponent->SetIsMontagePlaying(true);

	CharacterComponent->GetActionDatasOnehand(0).PlayMontage(this);
}

