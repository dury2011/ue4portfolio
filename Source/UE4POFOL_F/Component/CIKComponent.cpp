/* MEMO: CIKComponent.cpp �ڵ� ��ġ ���� �Ϸ� */
#include "Component/CIKComponent.h"
#include "Global.h"

#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"

//#define LOG_CIKCOMPONENT

UCIKComponent::UCIKComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCIKComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerCharacter = Cast<ACharacter>(GetOwner()); 
}

void UCIKComponent::CheckIK(FName InName, float& OutDistance, FRotator& OutRotation)
{
	FVector socket = OwnerCharacter->GetMesh()->GetSocketLocation(InName); // �Ű� ������ ���� ���� �̸��� ���� ���� ��ġ�� ��ȯ

	float z = OwnerCharacter->GetActorLocation().Z; // ������Ʈ ���� ������ Z ��ġ�� ��ȯ
	FVector start = FVector(socket.X, socket.Y, z); // start�� ������ X,Y ��ġ �׸��� ������ Z��ġ

	z = start.Z - OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - TraceDistance;
	FVector end = FVector(socket.X, socket.Y, z);

	TArray<AActor*> ignoreActors;
	ignoreActors.Add(OwnerCharacter); // ������Ʈ ������ �����ϰ� ��Ʈ�� ���� 

	FHitResult hitResult;
	UKismetSystemLibrary::LineTraceSingle
	(
		GetWorld(),
		start,
		end,
		ETraceTypeQuery::TraceTypeQuery5,
		true,
		ignoreActors,
		DrawDebug,
		hitResult,
		true,
		FLinearColor::Green,
		FLinearColor::Red
	); // ���� Ʈ���̽� �׸� 

	OutDistance = 0;
	OutRotation = FRotator::ZeroRotator; // �ϴ� 0���� �ʱ�ȭ ����

	CheckFalse(hitResult.bBlockingHit); // ���� Ʈ���̽��� ��Ʈ����� ������ ��

	float length = (hitResult.ImpactPoint - hitResult.TraceEnd).Size();
	OutDistance = length + OffsetDistance - TraceDistance; // ���� �Ÿ� ��ȯ 

	float roll = UKismetMathLibrary::DegAtan2(hitResult.ImpactNormal.Y, hitResult.ImpactNormal.Z);
	float pitch = -UKismetMathLibrary::DegAtan2(hitResult.ImpactNormal.X, hitResult.ImpactNormal.Z); // ���� ��ġ�� �� ��ȯ

	OutRotation = FRotator(pitch, 0, roll);

	return;
}

void UCIKComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float leftDistance;
	FRotator leftRotation;
	CheckIK(LeftSocket, leftDistance, leftRotation); // �޹��� ���� �Ÿ�, ȸ���� ��ȯ

	float rightDistance;
	FRotator rightRotation;
	CheckIK(RightSocket, rightDistance, rightRotation); // �������� ���� �Ÿ�, ȸ���� ��ȯ

	float offset = FMath::Min(leftDistance, rightDistance);
	FeetData.PelvisDistance.Z = UKismetMathLibrary::FInterpTo(FeetData.PelvisDistance.Z, offset, DeltaTime, InterpSpeed);

	FeetData.LeftDistance.X = UKismetMathLibrary::FInterpTo(FeetData.LeftDistance.X, (leftDistance - offset), DeltaTime, InterpSpeed);
	FeetData.RightDistance.X = UKismetMathLibrary::FInterpTo(FeetData.RightDistance.X, -(rightDistance - offset), DeltaTime, InterpSpeed);

	FeetData.LeftRotation = UKismetMathLibrary::RInterpTo(FeetData.LeftRotation, leftRotation, DeltaTime, InterpSpeed);
	FeetData.RightRotation = UKismetMathLibrary::RInterpTo(FeetData.RightRotation, rightRotation, DeltaTime, InterpSpeed); // ��ġ �̵� ���� ���� 

#ifdef LOG_CIKCOMPONENT
	CLog::Print(FeetData.LeftDistance, 1);
	CLog::Print(FeetData.RightDistance, 2);
	CLog::Print(FeetData.PelvisDistance, 3);
	CLog::Print(FeetData.LeftRotation, 4);
	CLog::Print(FeetData.RightRotation, 5);
#endif
}

