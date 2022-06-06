/* MEMO: CIKComponent.cpp 코드 배치 정리 완료 */
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
	FVector socket = OwnerCharacter->GetMesh()->GetSocketLocation(InName); // 매개 변수로 들어온 소켓 이름을 통해 소켓 위치를 반환

	float z = OwnerCharacter->GetActorLocation().Z; // 컴포넌트 소유 엑터의 Z 위치를 반환
	FVector start = FVector(socket.X, socket.Y, z); // start는 소켓의 X,Y 위치 그리고 엑터의 Z위치

	z = start.Z - OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - TraceDistance;
	FVector end = FVector(socket.X, socket.Y, z);

	TArray<AActor*> ignoreActors;
	ignoreActors.Add(OwnerCharacter); // 컴포넌트 소유자 제외하고 히트된 엑터 

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
	); // 라인 트레이스 그림 

	OutDistance = 0;
	OutRotation = FRotator::ZeroRotator; // 일단 0으로 초기화 해줌

	CheckFalse(hitResult.bBlockingHit); // 라인 트레이스에 히트결과가 없으면 끝

	float length = (hitResult.ImpactPoint - hitResult.TraceEnd).Size();
	OutDistance = length + OffsetDistance - TraceDistance; // 계산된 거리 반환 

	float roll = UKismetMathLibrary::DegAtan2(hitResult.ImpactNormal.Y, hitResult.ImpactNormal.Z);
	float pitch = -UKismetMathLibrary::DegAtan2(hitResult.ImpactNormal.X, hitResult.ImpactNormal.Z); // 계산된 피치와 롤 반환

	OutRotation = FRotator(pitch, 0, roll);

	return;
}

void UCIKComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float leftDistance;
	FRotator leftRotation;
	CheckIK(LeftSocket, leftDistance, leftRotation); // 왼발이 놓일 거리, 회전을 반환

	float rightDistance;
	FRotator rightRotation;
	CheckIK(RightSocket, rightDistance, rightRotation); // 오른발이 놓일 거리, 회전을 반환

	float offset = FMath::Min(leftDistance, rightDistance);
	FeetData.PelvisDistance.Z = UKismetMathLibrary::FInterpTo(FeetData.PelvisDistance.Z, offset, DeltaTime, InterpSpeed);

	FeetData.LeftDistance.X = UKismetMathLibrary::FInterpTo(FeetData.LeftDistance.X, (leftDistance - offset), DeltaTime, InterpSpeed);
	FeetData.RightDistance.X = UKismetMathLibrary::FInterpTo(FeetData.RightDistance.X, -(rightDistance - offset), DeltaTime, InterpSpeed);

	FeetData.LeftRotation = UKismetMathLibrary::RInterpTo(FeetData.LeftRotation, leftRotation, DeltaTime, InterpSpeed);
	FeetData.RightRotation = UKismetMathLibrary::RInterpTo(FeetData.RightRotation, rightRotation, DeltaTime, InterpSpeed); // 위치 이동 보간 적용 

#ifdef LOG_CIKCOMPONENT
	CLog::Print(FeetData.LeftDistance, 1);
	CLog::Print(FeetData.RightDistance, 2);
	CLog::Print(FeetData.PelvisDistance, 3);
	CLog::Print(FeetData.LeftRotation, 4);
	CLog::Print(FeetData.RightRotation, 5);
#endif
}

