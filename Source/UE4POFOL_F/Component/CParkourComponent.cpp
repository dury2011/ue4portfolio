/* MEMO: CParkourComponent.cpp 코드 배치 정리 완료 */
#include "Component/CParkourComponent.h"
#include "Global.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/CPlayer.h"

// #define DEBUG_PARKOURCOMPONENT

UCParkourComponent::UCParkourComponent()
	: bParkouring(false), 
	  bHideOrClimb(false)
{
	PrimaryComponentTick.bCanEverTick = true; 

	CHelpers::GetAsset<UDataTable>(&ParkourDataTable, "DataTable'/Game/FORUE4POFOL/Player/Asset/DT_PlayerParkour.DT_PlayerParkour'");
}

void UCParkourComponent::BeginPlay()
{
	Super::BeginPlay();

	SetIdleMode();
	
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	AnimInstance = Cast<UAnimInstance>(OwnerCharacter->GetMesh()->GetAnimInstance());
	
	////AnimInstance->OnMontageEnded.AddDynamic(this, &UCParkourComponent::CheckStartHideMode);
	AnimInstance->OnMontageEnded.AddDynamic(this, &UCParkourComponent::EndParkour);

	USceneComponent* ownerArrowComponent = CHelpers::GetComponent<USceneComponent>(OwnerCharacter, "ArrowGroup");
	TArray<USceneComponent*>childArrowComponents;
	
	ownerArrowComponent->GetChildrenComponents(false, childArrowComponents);

	for (int i = 0; i < (int32)EParkourArrowType::Max; i++)
		OwnerArrows[i] = Cast<UArrowComponent>(childArrowComponents[i]);

	TArray<FParkourData*> parkourDatas;
	ParkourDataTable->GetAllRows(" ", parkourDatas);

	for (int32 i = 0; i < (int32)EParkourType::Max; i++)
	{
		TArray<FParkourData> temp;

		for (auto parkourData : parkourDatas)
		{
			if (parkourData->Type == (EParkourType)i)
				temp.Add(*parkourData);
		}

		ParkourDataMap.Add((EParkourType)i, temp);
	}
}

void UCParkourComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	LineTraceHitActorCeil = nullptr;
	LineTraceHitActorCenter = nullptr;
	LineTraceHitActorFloor = nullptr;

	HitActorDistanceCeil = 0.0f;
	HitActorDistanceCenter = 0.0f;
	HitActorDistanceFloor = 0.0f;

	LineTraceCeil();
	LineTraceCenter();
	LineTraceLeftRight();
	LineTraceFloor();
	LineTraceLand();
	
	if(ParkourType != EParkourType::Max)
		CheckParkourState(ParkourType);

	if (OwnerCharacter->GetMovementComponent()->IsFalling() && !Done_CheckFallingHeight)
	{
		Done_CheckFallingHeight = true;
		
		FallingStartHeight = HitActorDistanceLand;
	}

	if (!OwnerCharacter->GetMovementComponent()->IsFalling() && Done_CheckFallingHeight)
	{
		Land();

		FallingStartHeight = 0.0f;
		Done_CheckFallingHeight = false;
		HitActorDistanceLand = 0.0f;
	}

//
//#ifdef DEBUG_PARKOURCOMPONENT
//	CLog::Print("HitActor_Ceil: " + LineTraceHitActorCeil->GetName(), 20);
//	CLog::Print("HitActor_Left: " + LineTraceHitActorLeft->GetName(), 21);
//	CLog::Print("HitActor_Center: " + LineTraceHitActorCenter->GetName(), 22);
//	CLog::Print("HitActor_Right: " + LineTraceHitActorRight->GetName(), 23);
//	CLog::Print("HitActor_Floor: " + LineTraceHitActorFloor->GetName(), 24);
//#endif DEBUG_PARKOURCOMPONENT
}

void UCParkourComponent::CreateLineTrace(EParkourArrowType InType)
{
	UArrowComponent* ownerArrows = OwnerArrows[(int32)InType];
	FLinearColor lineTraceColor = FLinearColor(ownerArrows->ArrowColor);
	FTransform lineTraceTransform = ownerArrows->GetComponentToWorld();
	FVector lineTraceStart = lineTraceTransform.GetLocation();

	FVector lineTraceEnd = FVector::ZeroVector;
	
	if (InType != EParkourArrowType::Land) 
		lineTraceEnd = lineTraceStart + FQuat(OwnerCharacter->GetActorRotation()).GetForwardVector() * LineTraceDistance;
	else
		lineTraceEnd = lineTraceStart + FQuat(OwnerCharacter->GetActorRotation()).GetUpVector() * -LineTraceDistance * 5000.0f;

	TArray<AActor*> lineTraceIgnoreActor;

	lineTraceIgnoreActor.Add(OwnerCharacter);

	UKismetSystemLibrary::LineTraceSingle
	(
		GetWorld(),
		lineTraceStart,
		lineTraceEnd,
		ETraceTypeQuery::TraceTypeQuery4,
		false,
		lineTraceIgnoreActor,
		DrawDebugType,
		HitResults[(int32)InType],
		true,
		lineTraceColor,
		FLinearColor::Green,
		0.0f
	);
}

void UCParkourComponent::LineTraceCeil()
{
	CreateLineTrace(EParkourArrowType::Ceil);

	FHitResult hitResult = HitResults[((int32)EParkourArrowType::Ceil)];

	if (hitResult.bBlockingHit)
	{
		UStaticMeshComponent* hitMesh = CHelpers::GetComponent<UStaticMeshComponent>(hitResult.GetActor());

		if (!!hitMesh)
		{
			LineTraceHitActorCeil = hitResult.GetActor();

			FVector minBound, maxBound;

			hitMesh->GetLocalBounds(minBound, maxBound);

			//MinBound = minBound;
			//MaxBound = maxBound;

			float x = FMath::Abs(minBound.X - maxBound.X);
			float y = FMath::Abs(minBound.Y - maxBound.Y);
			float z = FMath::Abs(minBound.Z - maxBound.Z);

			HitActorExtentCeil = FVector(x, y, z);
			HitActorDistanceCeil = hitResult.Distance;
			ToFrontYaw = UKismetMathLibrary::MakeRotFromX(-hitResult.ImpactNormal).Yaw;
		}
	}
}

void UCParkourComponent::LineTraceCenter()
{	
	CreateLineTrace(EParkourArrowType::Center);

	FHitResult hitResult = HitResults[((int32)EParkourArrowType::Center)];
	
	if (hitResult.bBlockingHit)
	{
		UStaticMeshComponent* hitMesh = CHelpers::GetComponent<UStaticMeshComponent>(hitResult.GetActor());

		if (!!hitMesh)
		{
			LineTraceHitActorCenter = hitResult.GetActor();

			FVector minBound, maxBound;
			
			hitMesh->GetLocalBounds(minBound, maxBound);
			
			MinBound = minBound;
			MaxBound = maxBound;

			float x = FMath::Abs(minBound.X - maxBound.X);
			float y = FMath::Abs(minBound.Y - maxBound.Y);
			float z = FMath::Abs(minBound.Z - maxBound.Z);

			HitActorExtentCenter = FVector(x, y, z);
			HitActorDistanceCenter = hitResult.Distance;
			//ToFrontYaw = UKismetMathLibrary::MakeRotFromX(-hitResult.ImpactNormal).Yaw;
		}
	}
}

void UCParkourComponent::LineTraceLeftRight()
{
	CreateLineTrace(EParkourArrowType::Left);
	CreateLineTrace(EParkourArrowType::Right);

	FHitResult hitResultLeft = HitResults[((int32)EParkourArrowType::Left)];
	FHitResult hitResultRight = HitResults[((int32)EParkourArrowType::Right)];

	if (hitResultLeft.bBlockingHit)
	{
		UStaticMeshComponent* hitMesh = CHelpers::GetComponent<UStaticMeshComponent>(hitResultLeft.GetActor());

		if (!!hitMesh)
		{
			LineTraceHitActorLeft = hitResultLeft.GetActor();

			FVector minBound, maxBound;

			hitMesh->GetLocalBounds(minBound, maxBound);

			//MinBound = minBound;
			//MaxBound = maxBound;

			float x = FMath::Abs(minBound.X - maxBound.X);
			float y = FMath::Abs(minBound.Y - maxBound.Y);
			float z = FMath::Abs(minBound.Z - maxBound.Z);

			HitActorExtentLeft = FVector(x, y, z);
			HitActorDistanceLeft = hitResultLeft.Distance;
			//ToFrontYaw = UKismetMathLibrary::MakeRotFromX(-hitResultLeft.ImpactNormal).Yaw;
		}
	}

	if (hitResultRight.bBlockingHit)
	{
		UStaticMeshComponent* hitMesh = CHelpers::GetComponent<UStaticMeshComponent>(hitResultRight.GetActor());

		if (!!hitMesh)
		{
			LineTraceHitActorRight = hitResultRight.GetActor();

			FVector minBound, maxBound;

			hitMesh->GetLocalBounds(minBound, maxBound);

			//MinBound = minBound;
			//MaxBound = maxBound;

			float x = FMath::Abs(minBound.X - maxBound.X);
			float y = FMath::Abs(minBound.Y - maxBound.Y);
			float z = FMath::Abs(minBound.Z - maxBound.Z);

			HitActorExtentRight = FVector(x, y, z);
			HitActorDistanceRight = hitResultRight.Distance;
			//ToFrontYaw = UKismetMathLibrary::MakeRotFromX(-hitResultRight.ImpactNormal).Yaw;
		}
	}
}

void UCParkourComponent::LineTraceFloor()
{
	CreateLineTrace(EParkourArrowType::Floor);

	FHitResult hitResult = HitResults[((int32)EParkourArrowType::Floor)];

	if (hitResult.bBlockingHit)
	{
		UStaticMeshComponent* hitMesh = CHelpers::GetComponent<UStaticMeshComponent>(hitResult.GetActor());

		if (!!hitMesh)
		{
			LineTraceHitActorFloor = hitResult.GetActor();

			FVector minBound, maxBound;

			hitMesh->GetLocalBounds(minBound, maxBound);

			//MinBound = minBound;
			//MaxBound = maxBound;

			float x = FMath::Abs(minBound.X - maxBound.X);
			float y = FMath::Abs(minBound.Y - maxBound.Y);
			float z = FMath::Abs(minBound.Z - maxBound.Z);

			HitActorExtentFloor = FVector(x, y, z);
			HitActorDistanceFloor = hitResult.Distance;
			//ToFrontYaw = UKismetMathLibrary::MakeRotFromX(-hitResult.ImpactNormal).Yaw;
		}
	}
}

void UCParkourComponent::LineTraceLand()
{
	CreateLineTrace(EParkourArrowType::Land);

	FHitResult hitResult = HitResults[((int32)EParkourArrowType::Land)];
	
	if (hitResult.bBlockingHit)
	{
		UStaticMeshComponent* hitMesh = CHelpers::GetComponent<UStaticMeshComponent>(hitResult.GetActor());

		if (!!hitMesh)
		{
			LineTraceHitActorLand = hitResult.GetActor();

			FVector minBound, maxBound;

			hitMesh->GetLocalBounds(minBound, maxBound);

			//MinBound = minBound;
			//MaxBound = maxBound;

			float x = FMath::Abs(minBound.X - maxBound.X);
			float y = FMath::Abs(minBound.Y - maxBound.Y);
			float z = FMath::Abs(minBound.Z - maxBound.Z);

			HitActorExtentLand = FVector(x, y, z);
			HitActorDistanceLand = hitResult.Distance;
			//ToFrontYaw = UKismetMathLibrary::MakeRotFromX(-hitResult.ImpactNormal).Yaw;
		}
	}
}

void UCParkourComponent::DoParkour()
{
	//CheckTrue(bParkouring);
	//
	//if (CheckRollMode())
	//{
	//	OwnerCharacter->bUseControllerRotationYaw = false;
	//	
	//	OwnerCharacter->SetActorRotation
	//	(
	//		UKismetMathLibrary::Conv_VectorToRotator
	//		(
	//			OwnerCharacter->GetVelocity().GetSafeNormal2D()
	//		)
	//	);

	//	return;
	//}

	if (CheckSlideMode())
	{
		Cast<ACPlayer>(OwnerCharacter)->ParkourTargetRot = OwnerCharacter->GetLastMovementInputVector().Rotation();

		BeginSlide();
		
		if (LineTraceHitActorCeil)
		{
			ParkourObstacle = LineTraceHitActorCeil;
			ParkourObstacle->SetActorEnableCollision(false);
		}

		OwnerCharacter->bUseControllerRotationYaw = false;

		return;
	}

	////if (CheckHideMode())
	////{
	////	BeginHide();

	////	return;
	////}

	if (CheckFlipMode())
	{
		Cast<ACPlayer>(OwnerCharacter)->ParkourTargetRot = OwnerCharacter->GetLastMovementInputVector().Rotation();

		if (LineTraceHitActorCenter || LineTraceHitActorFloor)
		{
			if (LineTraceHitActorCenter)
			{
				ParkourObstacle = LineTraceHitActorCenter;
				ParkourObstacle->SetActorEnableCollision(false);
			}
			else if (LineTraceHitActorFloor)
			{
				ParkourObstacle = LineTraceHitActorFloor;
				ParkourObstacle->SetActorEnableCollision(false);
			}
	
			OwnerCharacter->bUseControllerRotationYaw = false;

			BeginFlip();

			return;
		}
	}

	if (CheckBeginClimbUpMode())
	{
		Cast<ACPlayer>(OwnerCharacter)->ParkourTargetRot = OwnerCharacter->GetLastMovementInputVector().Rotation();
		
		OwnerCharacter->bUseControllerRotationYaw = false;

		BeginClimbUp();

		return;
	}
	
	if (CheckBeginClimbDownMode())
	{
		OwnerCharacter->bUseControllerRotationYaw = false;
		//ParkourObstacle->SetActorEnableCollision(false);

		BeginClimbDown();

		return;
	}

	Cast<ACPlayer>(OwnerCharacter)->ParkourTargetRot = OwnerCharacter->GetLastMovementInputVector().Rotation();
	BeginRoll();
}

void UCParkourComponent::SetIdleMode()
{
	bParkouring = false;
	
	ChangeType(EParkourType::Max);
}	

void UCParkourComponent::SetRollMode()
{
	ChangeType(EParkourType::Roll);
}

void UCParkourComponent::SetSlideMode()
{
	ChangeType(EParkourType::Slide);
}

void UCParkourComponent::SetJumpMode()
{
	//ChangeType(EParkourType::Jump);
}

void UCParkourComponent::SetHideMode()
{
	//ChangeType(EParkourType::Hide);
}

void UCParkourComponent::SetFlipMode()
{
	ChangeType(EParkourType::Flip);
}

void UCParkourComponent::SetBeginClimbUpMode()
{
	ChangeType(EParkourType::BeginClimbUp);
}

void UCParkourComponent::SetBeginClimbDownMode()
{
	ChangeType(EParkourType::BeginClimbDown);
}

void UCParkourComponent::SetClimbingMode()
{
	ChangeType(EParkourType::Climbing);
}

void UCParkourComponent::SetEndClimbUpMode()
{
	ChangeType(EParkourType::EndClimbUp);
}

void UCParkourComponent::SetEndClimbDownMode()
{
	ChangeType(EParkourType::EndClimbDown);
}

void UCParkourComponent::SetEndClimbJumpMode()
{
	ChangeType(EParkourType::EndClimbJump);
}

bool UCParkourComponent::CheckRollMode()
{
	//if (!LineTraceHitActorCeil && !LineTraceHitActorCenter && !LineTraceHitActorFloor)
	//{
	//	return true;
	//}
	//else
	//	return false;

	return false;
}

bool UCParkourComponent::CheckSlideMode()
{
	if (LineTraceHitActorCeil && !LineTraceHitActorCenter && !LineTraceHitActorFloor)
		if (HitActorDistanceCeil <= 200.0f)
			return true;
		else return false;
	else
		return false;
}

bool UCParkourComponent::CheckHideMode()
{
	return false;
}

bool UCParkourComponent::CheckFlipMode()
{
	if (!LineTraceHitActorCeil && (LineTraceHitActorCenter || LineTraceHitActorFloor))
	{
		if (HitActorDistanceCenter <= 200.f)
			return true;
		else
			return false;
	}
	else
		return false;
}

bool UCParkourComponent::CheckBeginClimbUpMode()
{
	if (LineTraceHitActorCeil && LineTraceHitActorCenter && LineTraceHitActorFloor)
		if (HitActorDistanceCenter < 50.0f)
			return true;
		else return false;
	else return false;
}

bool UCParkourComponent::CheckBeginClimbDownMode()
{
	if (!LineTraceHitActorCeil && !LineTraceHitActorCenter && !LineTraceHitActorFloor && LineTraceHitActorLand)
		if (HitActorDistanceLand >= 200.0f)
			return true;
		else return false;
	else return false;
}

void UCParkourComponent::CheckParkourState(EParkourType InCurrentType)
{
	switch(InCurrentType)
	{
		case EParkourType::Climbing :
		{
			//if (!LineTraceHitActorCenter)
				//AbortClimb();

			if (HitActorDistanceCeil >= 100.0f || !LineTraceHitActorCeil)
				CompleteClimb();

			break;
		}
	
		case EParkourType::EndClimbJump :
		{
			if (HitActorDistanceLand <= 50.f)
			{
				SetIdleMode();

				break;
			}
		}

		case EParkourType::BeginClimbDown :
		{
			if (LineTraceHitActorCeil)
				Climbing();

			break;
		}

		default:
			break;
	}
}

void UCParkourComponent::BeginRoll()
{
	SetRollMode();

	const TArray<FParkourData>* parkourDatas = ParkourDataMap.Find(EParkourType::Roll);
	
	bParkouring = true;

	OwnerCharacter->PlayAnimMontage((*parkourDatas)[0].Montage, (*parkourDatas)[0].PlayRatio, (*parkourDatas)[0].SectionName);
	
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void UCParkourComponent::EndRoll()
{
	SetIdleMode();

	//GLog->Log("Roll");
}

void UCParkourComponent::BeginSlide()
{
	SetSlideMode();

	const TArray<FParkourData>* parkourDatas = ParkourDataMap.Find(EParkourType::Slide);

	OwnerCharacter->PlayAnimMontage((*parkourDatas)[0].Montage, (*parkourDatas)[0].PlayRatio, (*parkourDatas)[0].SectionName);

	bParkouring = true;

	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
}

void UCParkourComponent::EndSlide()
{
	CheckFalse(IsSlideMode());

	SetIdleMode();

	if (ParkourObstacle)
		ParkourObstacle->SetActorEnableCollision(true);

	GLog->Log("Slide");
}

void UCParkourComponent::Land()
{
	if (FallingStartHeight >= FallingHighPlayDistance)
	{
		const TArray<FParkourData>* parkourDatas = ParkourDataMap.Find(EParkourType::HighFalling);

		OwnerCharacter->PlayAnimMontage((*parkourDatas)[0].Montage, (*parkourDatas)[0].PlayRatio, (*parkourDatas)[0].SectionName);

		SetIdleMode();

		bParkouring = true;

		OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
}

void UCParkourComponent::BeginJump()
{
	//if (OwnerCharacter->JumpCurrentCount >= 2)
	//	return;
	//
	//SetJumpMode();
	//
	//bHideOrClimb = true;
	//bBeginJump = true;

	//OwnerCharacter->Jump();
	//OwnerCharacter->JumpCurrentCount += 1;

 //	//OwnerCharacter->LaunchCharacter(OwnerCharacter->GetActorUpVector() * 1000.0f, true, true);
	//OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
}

void UCParkourComponent::EndJump()
{
	//bHideOrClimb = false;
	//bBeginJump = false;

	//OwnerCharacter->JumpCurrentCount = 0;
	//OwnerCharacter->StopJumping();
	//OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	//SetIdleMode();
}

void UCParkourComponent::BeginHide()
{
	//SetHideMode();

	//bParkouring = true;
}

void UCParkourComponent::EndHide()
{
	//CheckFalse(IsHideMode());
	//
	//SetIdleMode();

	//GLog->Log("Hide");
}

void UCParkourComponent::BeginFlip()
{
	SetFlipMode();

	const TArray<FParkourData>* parkourDatas = ParkourDataMap.Find(EParkourType::Flip);
	
	bParkouring = true;
	
	OwnerCharacter->PlayAnimMontage((*parkourDatas)[0].Montage, (*parkourDatas)[0].PlayRatio, (*parkourDatas)[0].SectionName);
}

void UCParkourComponent::EndFlip()
{
	CheckFalse(IsFlipMode());
	
	SetIdleMode();

	if (ParkourObstacle)
		ParkourObstacle->SetActorEnableCollision(true);

	GLog->Log("Flip");
}

void UCParkourComponent::BeginClimbUp()
{
	SetBeginClimbUpMode();

	//OwnerCharacter->SetActorRotation(FRotator(0, ToFrontYaw, 0));

	const TArray<FParkourData>* parkourDatas = ParkourDataMap.Find(EParkourType::BeginClimbUp);
	OwnerCharacter->PlayAnimMontage((*parkourDatas)[0].Montage, (*parkourDatas)[0].PlayRatio, (*parkourDatas)[0].SectionName);
	
	//OwnerCharacter->SetActorLocation(HitResults[(int32)EParkourArrowType::Center].ImpactPoint + 10.f);
	
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

	bParkouring = true;
	
	GLog->Log("Begin Climb");
}

void UCParkourComponent::BeginClimbDown()
{
	SetBeginClimbDownMode();

	//OwnerCharacter->SetActorLocation(HitResults[(int32)EParkourArrowType::Center].ImpactPoint + 10.f);
	//OwnerCharacter->SetActorRotation(FRotator(0, -180.0f, 0));

	const TArray<FParkourData>* parkourDatas = ParkourDataMap.Find(EParkourType::BeginClimbDown);

	OwnerCharacter->PlayAnimMontage((*parkourDatas)[0].Montage, (*parkourDatas)[0].PlayRatio, (*parkourDatas)[0].SectionName);

	bParkouring = true;

	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

	GLog->Log("Begin Climb");
}

void UCParkourComponent::EndClimbUp()
{
	SetIdleMode();

	GLog->Log("End Climb");
}

void UCParkourComponent::EndClimbDown()
{
	SetIdleMode();

	GLog->Log("End Climb");
}

void UCParkourComponent::CompleteClimb()
{
	CheckFalse(IsClimbingMode());

	SetEndClimbUpMode();

	const TArray<FParkourData>* parkourDatas = ParkourDataMap.Find(EParkourType::EndClimbUp);

	OwnerCharacter->PlayAnimMontage((*parkourDatas)[0].Montage, (*parkourDatas)[0].PlayRatio, (*parkourDatas)[0].SectionName);

	bHideOrClimb = false;
	
	GLog->Log("Complete Climb");
}

void UCParkourComponent::Climbing()
{
	bParkouring = true;
	
	SetClimbingMode();
}

void UCParkourComponent::AbortClimb()
{
	//CheckFalse(IsClimbingMode());

	//SetEndClimbJumpMode();
	//
	//const TArray<FParkourData>* parkourDatas = ParkourDataMap.Find(EParkourType::EndClimbJump);

	//OwnerCharacter->PlayAnimMontage((*parkourDatas)[0].Montage, (*parkourDatas)[0].PlayRatio, (*parkourDatas)[0].SectionName);
	//OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
	//
	//bHideOrClimb = false;

	//GLog->Log("Abort Climb");
}

void UCParkourComponent::EndParkour(UAnimMontage* InMontage, bool InIsInterrupted)
{
	//if (IsRollMode())
	//	EndRoll();
	if (IsSlideMode())
		EndSlide();
	else if (IsFlipMode())
		EndFlip();
	else if (IsEndClimbUpMode())
		EndClimbUp();
	else if (IsEndClimbDownMode())
		EndClimbDown();
	else if (IsBeginClimbUpMode())
		Climbing();
	else if (IsBeginClimbDownMode())
		Climbing();

	
	CheckTrue(IsBeginClimbUpMode() || IsBeginClimbDownMode() || IsClimbingMode());
	
	OwnerCharacter->bUseControllerRotationYaw = true;
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	bParkouring = false;
}

void UCParkourComponent::ChangeType(EParkourType InNewType)
{
	PrevParkourType = ParkourType;
	ParkourType = InNewType;

	if (OnParkourTypeChanged.IsBound())
		OnParkourTypeChanged.Broadcast(InNewType);
}