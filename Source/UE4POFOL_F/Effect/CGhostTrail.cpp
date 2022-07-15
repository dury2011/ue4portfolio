/* MEMO: CGhostTrail.cpp 코드 배치 정리 완료 */
#include "CGhostTrail.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"

ACGhostTrail::ACGhostTrail()
{
	CHelpers::CreateComponent<UPoseableMeshComponent>(this, &Poseable, "Poseable");
	CHelpers::GetAsset<UMaterialInstanceConstant>(&Material, "MaterialInstanceConstant'/Game/EffectMaster/M_GhostTrail_Inst.M_GhostTrail_Inst'");
}

void ACGhostTrail::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(3);
	
	Poseable->SetVisibility(false);
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	Poseable->SetSkeletalMesh(OwnerCharacter->GetMesh()->SkeletalMesh);
	Poseable->CopyPoseFromSkeletalComponent(OwnerCharacter->GetMesh());
	DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);

	for (int32 i = 0; i < OwnerCharacter->GetMesh()->SkeletalMesh->Materials.Num(); i++)
		Poseable->SetMaterial(i, DynamicMaterial);
		
	SetActorRotation(OwnerCharacter->GetActorRotation() + FRotator(0, -90, 0));
	
	//FTimerHandle startTimerHandle;
	//
	//FTimerDelegate startDelegate = FTimerDelegate::CreateLambda([=]()
	//{
	//	if (Poseable->IsVisible() == false)
	//		Poseable->ToggleVisibility();
	//
	//	Poseable->CopyPoseFromSkeletalComponent(OwnerCharacter->GetMesh());
	//});
	//
	//GetWorld()->GetTimerManager().SetTimer(startTimerHandle, startDelegate, Interval, true, StartDelay);

	FTimerHandle destoryTimer;
	GetWorld()->GetTimerManager().SetTimer(destoryTimer, this, &ACGhostTrail::DeleteTrail, ExistingTime, true);
}

ACGhostTrail* ACGhostTrail::SpawnGhostTrail(ACharacter* InSpawner, TSubclassOf<ACGhostTrail> InGhostTrailClass)
{
	FActorSpawnParameters params;
	params.Owner = InSpawner;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector vector = InSpawner->GetActorLocation();
	vector.Z -= 90;

	FTransform transform;
	transform.SetLocation(vector);

	return InSpawner->GetWorld()->SpawnActor<ACGhostTrail>(InGhostTrailClass, transform, params);
}

void ACGhostTrail::ToggleOn()
{
	Poseable->SetVisibility(true);
}

void ACGhostTrail::ToggleOff()
{
	Poseable->SetVisibility(false);
}

void ACGhostTrail::DeleteTrail()
{
	if(Poseable)
		Poseable->DestroyComponent();
}
