/* MEMO: CAniminstance.cpp �ڵ� ��ġ ���� �Ϸ� */
#include "Animation/CAnimInstance.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Enemy/CEnemy.h"
#include "Component/CCharacterComponent.h"
#include "Player/CPlayer.h"

void UCAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	CheckNull(OwnerCharacter);

	CharacterComponent = Cast<UCCharacterComponent>(OwnerCharacter->GetComponentByClass(UCCharacterComponent::StaticClass()));
	IKComponent = Cast<UCIKComponent>(OwnerCharacter->GetComponentByClass(UCIKComponent::StaticClass()));

	if (CharacterComponent)
	{
		CharacterComponent->OnStateTypeChange.AddDynamic(this, &UCAnimInstance::OnStateTypeChanged);
		CharacterComponent->OnWeaponTypeChange.AddDynamic(this, &UCAnimInstance::OnWeaponTypeChanged);
	}

	//StateComponent = CHelpers::GetComponent<UCStateComponent>(OwnerCharacter);
	//WeaponComponent = CHelpers::GetComponent<UCWeaponComponent>(OwnerCharacter);
	//ParkourComponent = CHelpers::GetComponent<UCParkourComponent>(OwnerCharacter);
	//
	//if (!!WeaponComponent)
	//	WeaponComponent->OnWeaponTypeChanged.AddDynamic(this, &UCAnimInstance::OnWeaponTypeChanged);
	//
	//if (!!ParkourComponent)
	//	ParkourComponent->OnParkourTypeChanged.AddDynamic(this, &UCAnimInstance::OnParkourTypeChanged);
}

void UCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	CheckNull(OwnerCharacter);
	
	ACPlayer* player = Cast<ACPlayer>(OwnerCharacter);
	UCParkourComponent* parkourComponent = CHelpers::GetComponent<UCParkourComponent>(player);

	if (player)
	{
		IsSpellTravel = player->GetIsSpellTravel();
		IsParkouring = parkourComponent->GetbParkouring();
		IsClimbing = player->GetIsParkouringClimbing();
	}

	if (IKComponent)
		FeetData = IKComponent->GetFeetData();

	//FwdSpeed = UKismetMathLibrary::Dot_VectorVector(OwnerCharacter->GetVelocity(), OwnerCharacter->GetActorForwardVector());
	//RSpeed = UKismetMathLibrary::Dot_VectorVector(OwnerCharacter->GetVelocity(), OwnerCharacter->GetActorRightVector());
	Speed = OwnerCharacter->GetVelocity().Size2D();
	bInAir = OwnerCharacter->GetMovementComponent()->IsFalling();
	Pitch = UKismetMathLibrary::NormalizedDeltaRotator(OwnerCharacter->GetBaseAimRotation(), OwnerCharacter->GetActorRotation()).Pitch;
	Roll = UKismetMathLibrary::NormalizedDeltaRotator(OwnerCharacter->GetBaseAimRotation(), OwnerCharacter->GetActorRotation()).Roll;
	Yaw = UKismetMathLibrary::NormalizedDeltaRotator(OwnerCharacter->GetBaseAimRotation(), OwnerCharacter->GetActorRotation()).Yaw;
	RotationLastTick = OwnerCharacter->GetActorRotation();
	YawDelta = UKismetMathLibrary::FInterpTo(YawDelta, UKismetMathLibrary::NormalizedDeltaRotator(RotationLastTick, OwnerCharacter->GetActorRotation()).Yaw / DeltaSeconds / 10.0f, DeltaSeconds, 0.0f);
	IsRunning = (Speed >= 550.0f);

	if (!IsClimbing)
		Direction = CalculateDirection(OwnerCharacter->GetVelocity(), OwnerCharacter->GetControlRotation());
	else
		Direction = player->ClimbingInput;
	
	if (OwnerCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size2D() > 0.0f)
		bAccelerating = true;
	else
		bAccelerating = false;

	// TODO: �ʿ� ���� �� ����
	if(GetCurveValue(FName("FullBody")))
		bFullBody = true;
	else 
		bFullBody = false;

	
	//if (UKismetMathLibrary::NearlyEqual_FloatFloat(Yaw, 175.0f, 0.1f))
	//	bTurnBack = true;
	//else if (UKismetMathLibrary::NearlyEqual_FloatFloat(Yaw, -175.0f, 0.1f))
	//	bTurnBack = true;
	//else
	//	bTurnBack = false;
}

//void UCAnimInstance::OnWeaponTypeChanged(EWeaponType InPrevType, EWeaponType InNewType)
//{
//	ActiveWeaponType = InNewType;
//}
//
//void UCAnimInstance::OnParkourTypeChanged(EParkourType InNewType)
//{
//	ActiveParkourType = InNewType;
//}
//
// MEMO: NativeBeginPlayer���� ParkourComponent�� Get�ϹǷ� 
// * �÷��� ���� �� ���� �������� ���Ƿ� �� üũ�� �ؾ��Ѵ�. */
//bool UCAnimInstance::IsHideOrClimbMode()
//{
//	if (ParkourComponent)
//	{
//		return ParkourComponent->GetbHideOrClimb();
//	}
//	else
//		return false;
//}

void UCAnimInstance::OnStateTypeChanged(EStateType InCurrentType, EStateType InPreviousType)
{
	CurrentStateType = InCurrentType;
	PreviousStateType = InPreviousType;
}

void UCAnimInstance::OnWeaponTypeChanged(EWeaponType InCurrentType, EWeaponType InPreviousType)
{
	CurrentWeaponType = InCurrentType;
	PreviousWeaponType = InPreviousType;
}
