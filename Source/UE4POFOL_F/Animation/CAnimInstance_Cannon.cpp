#include "Animation/CAnimInstance_Cannon.h"
#include "Global.h"
#include "Player/CCannon.h"

void UCAnimInstance_Cannon::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	Cannon = Cast<ACCannon>(TryGetPawnOwner());
	CheckNull(Cannon);
	
	Cannon->OnCannonStateTypeChange.AddDynamic(this, &UCAnimInstance_Cannon::CannonStateTypeChange);
}

void UCAnimInstance_Cannon::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	CheckNull(Cannon);

	Pitch = UKismetMathLibrary::NormalizedDeltaRotator(Cannon->GetBaseAimRotation(), Cannon->GetActorRotation()).Pitch;
	Roll = UKismetMathLibrary::NormalizedDeltaRotator(Cannon->GetBaseAimRotation(), Cannon->GetActorRotation()).Roll;
	Yaw = UKismetMathLibrary::NormalizedDeltaRotator(Cannon->GetBaseAimRotation(), Cannon->GetActorRotation()).Yaw;
}

void UCAnimInstance_Cannon::CannonStateTypeChange(ECannonStateType InPreviousType, ECannonStateType InCurrentType)
{
	CurrentCannonStateType = InCurrentType;
	PreviousCannonStateType = InPreviousType;
}
