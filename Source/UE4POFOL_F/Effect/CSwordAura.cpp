/* MEMO: CSwordAura.cpp 코드 배치 정리 완료 */
#include "Effect/CSwordAura.h"
#include "Global.h"

#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"

ACSwordAura::ACSwordAura()
{
	PrimaryActorTick.bCanEverTick = false;

	CHelpers::CreateComponent<USceneComponent>(this, &Root, "Root");
	CHelpers::CreateComponent<UNiagaraComponent>(this, &Niagara, "Niagara", Root);
	CHelpers::CreateComponent<UBoxComponent>(this, &Box, "Box", Root);

	UNiagaraSystem* niagara;
	CHelpers::GetAsset<UNiagaraSystem>(&niagara, "NiagaraSystem'/Game/EffectMaster/SwordAura/P_SwordAura.P_SwordAura'");
	Niagara->SetAsset(niagara);

	Box->SetRelativeLocation(FVector(280, 0, 130));
	Box->SetRelativeScale3D(FVector(6, 3, 4));
}

void ACSwordAura::BeginPlay()
{
	Super::BeginPlay();

	Niagara->SetNiagaraVariableObject("BP_Scale_Callback", this);

	InitialBoxScale = Box->GetRelativeScale3D();
	Niagara->OnSystemFinished.AddDynamic(this, &ACSwordAura::OnSystemFinished);

	Box->OnComponentBeginOverlap.AddDynamic(this, &ACSwordAura::OnComponentBeginOverlap);
	Box->OnComponentEndOverlap.AddDynamic(this, &ACSwordAura::OnComponentEndOverlap);
}

void ACSwordAura::BeginDestroy()
{
	Super::BeginDestroy();
}

void ACSwordAura::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACSwordAura::ReceiveParticleData_Implementation(const TArray<FBasicParticleData>& Data, UNiagaraSystem* NiagaraSystem)
{
	FVector scale = Data[0].Position * Root->GetRelativeScale3D() * InitialBoxScale;
	Box->SetRelativeScale3D(scale);
}

void ACSwordAura::OnSystemFinished(class UNiagaraComponent* PSystem)
{
	Destroy();
}

void ACSwordAura::ToggleOn()
{
	Niagara->SetVisibility(false);
}

void ACSwordAura::ToggleOff()
{
	Niagara->SetVisibility(true);
}

void ACSwordAura::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CheckTrue(GetOwner() == OtherActor);

	if (OnSwordAuraBoxBeginOverlap.IsBound())
		OnSwordAuraBoxBeginOverlap.Broadcast(Cast<ACharacter>(GetOwner()), Cast<AActor>(Box), Cast<ACharacter>(OtherActor));
}

void ACSwordAura::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CheckTrue(GetOwner() == OtherActor);

	if (OnSwordAuraBoxEndOverlap.IsBound())
		OnSwordAuraBoxEndOverlap.Broadcast(Cast<ACharacter>(GetOwner()), Cast<AActor>(Box), Cast<ACharacter>(OtherActor));
}
