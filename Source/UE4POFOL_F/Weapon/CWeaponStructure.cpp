#include "Weapon/CWeaponStructure.h"
#include "Global.h"
#include "GameFramework/Character.h"
//#include "Component/CStateComponent.h"
#include "Component/CCharacterComponent.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimMontage.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Effect/CGhostTrail.h"

//#define LOG_FHITDATA

//UAnimMontage* UCWeaponStructure::DefaultHitMontage = NULL;

UCWeaponStructure::UCWeaponStructure()
{
	//CHelpers::GetAsset<UAnimMontage>(&DefaultHitMontage, "AnimMontage'/Game/FORUE4POFOL/Enemy/Montages/Hitted/Enemy_Hitted_Montage.Enemy_Hitted_Montage'");
}

void FActionData::PlayMontage(class ACharacter* InOwnerCharacter)
{
	UCCharacterComponent* characterComponent = Cast<UCCharacterComponent>(InOwnerCharacter->GetComponentByClass(UCCharacterComponent::StaticClass()));

	if (characterComponent)
	{
		bCanMove ? characterComponent->SetbCanMove(true) : characterComponent->SetbCanMove(false);
		bFixedCamera ? characterComponent->SetbFixedCamera(true) : characterComponent->SetbFixedCamera(false);
	}
	
	InOwnerCharacter->PlayAnimMontage(Montage, PlayRatio, SectionName);
}

void FActionData::StopMontage(class ACharacter* InOwnerCharacter)
{
	//UCStateComponent* stateComponent = CHelpers::GetComponent<UCStateComponent>(InOwnerCharacter);

	//if (!!stateComponent)
	//{
	//	bCanMove ? stateComponent->SetMove() : stateComponent->SetStop();
	//	bFixedCamera ? stateComponent->EnableFixCamera() : stateComponent->DisableFixCamera();
	//}

	//InOwnerCharacter->StopAnimMontage(Montage);
}

void FActionData::PauseMontage(class ACharacter* InOwnerCharacter, class UAnimInstance* InAnimInstance)
{
	//UCStateComponent* stateComponent = CHelpers::GetComponent<UCStateComponent>(InOwnerCharacter);

	//if (!!stateComponent)
	//{
	//	bCanMove ? stateComponent->SetMove() : stateComponent->SetStop();
	//	bFixedCamera ? stateComponent->EnableFixCamera() : stateComponent->DisableFixCamera();
	//}

	//InAnimInstance->Montage_Pause(Montage);
	////InOwnerCharacter->GetRootMotionAnimMontageInstance()->Pause();
}

void FActionData::ResumeMontage(class ACharacter* InOwnerCharacter, class UAnimInstance* InAnimInstance)
{
	//UCStateComponent* stateComponent = CHelpers::GetComponent<UCStateComponent>(InOwnerCharacter);

	//if (!!stateComponent)
	//{
	//	bCanMove ? stateComponent->SetMove() : stateComponent->SetStop();
	//	bFixedCamera ? stateComponent->EnableFixCamera() : stateComponent->DisableFixCamera();
	//}

	//InAnimInstance->Montage_Resume(Montage);
	////InOwnerCharacter->GetRootMotionAnimMontageInstance()->Play()
}

void FActionData::EndAction(ACharacter * InOwnerCharacter)
{
	////if (!!BackupGhostTrail)
	////	BackupGhostTrail->Destroy();

	//UCStateComponent* stateComponent = CHelpers::GetComponent<UCStateComponent>(InOwnerCharacter);
	//
	//if (!!stateComponent)
	//{
	//	stateComponent->SetMove();
	//	stateComponent->DisableFixCamera();
	//}

	//if (!!stateComponent)
	//	stateComponent->SetEquipMode();
}

//void FDoActionData::SpawnGhostTrail(class ACharacter* InOwner)
//{
//	CheckNull(GhostTrailClass);
//
//	FActorSpawnParameters params;
//	params.Owner = InOwner;
//	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//
//	FVector vector = InOwner->GetActorLocation();
//	vector.Z -= 90;
//
//	FTransform transform;
//	transform.SetLocation(vector);
//
//	BackupGhostTrail = InOwner->GetWorld()->SpawnActor<ACGhostTrail>(GhostTrailClass, transform, params);
//}

void FDamageData::PlayEffect(UWorld * InWorld, ACharacter * InOwner)
{
	CheckNull(Effect);

	FTransform transform = InOwner->GetTransform();

	FVector ownerLocation = InOwner->GetActorLocation();
	//ownerLocation += InOwner->GetActorRotation().RotateVector(transform.GetLocation());

	transform.SetLocation(ownerLocation);
	transform.SetRotation(FQuat(InOwner->GetActorRotation()));

	CHelpers::PlayEffect(InWorld, Effect, transform);
}

void FDamageData::PlayHitStop(UWorld * InWorld)
{
	//CheckTrue(FMath::IsNearlyZero(StopTime));

	//TArray<AActor*> movables;
	//
	//for (AActor* actor : InWorld->GetCurrentLevel()->Actors)
	//{
	//	if (actor == nullptr)
	//		continue;

	//	if (actor->IsRootComponentMovable() == false)
	//		continue;
	//	
	//	actor->CustomTimeDilation = 0.1f;
	//	movables.Add(actor);
	//}
	//
	//FTimerHandle timerHandle;
	//FTimerDelegate timerDelegate = FTimerDelegate::CreateLambda([=]()
	//{
	//	for (AActor* actor : movables)
	//		actor->CustomTimeDilation = 1;
	//});
	//InWorld->GetTimerManager().SetTimer(timerHandle, timerDelegate, StopTime, false);
}

void FDamageData::PlaySoundCue(UWorld * InWorld, const FVector & InLocation)
{
	//CheckNull(SoundCue);

	//UGameplayStatics::SpawnSoundAtLocation(InWorld, SoundCue, InLocation);
}

void FDamageData::SendDamage(ACharacter * InAttacker, AActor * InCauser, ACharacter * InOtherCharacter)
{
	FActionDamageEvent actionDamageEvent; // EXPLAIN: FDamageData(에님 몽타주, 이펙트 관련) 객체만 있는 구조체 
	
	actionDamageEvent.DamageData = this; // EXPLAIN: 이 함수를 호출한 객체를 할당, 즉, HitData를 통해 호출하므로 HitData를 넣어줌
	
	float damageFloat = Power + FMath::RandRange(200.0f, 300.0f);

	InOtherCharacter->TakeDamage(damageFloat, actionDamageEvent, InAttacker->GetController(), InCauser);

#ifdef LOG_FDAMAGEDATA
	CLog::Print("FDamageData::SendDamage() fixPower: " + (char)fixPower, 15);
#endif LOG_FDAMAGEDATA
}

void FDamageData::PlayMontage(ACharacter * InOwnerCharacter)
{
	//UCStateComponent* stateComponent  = CHelpers::GetComponent<UCStateComponent>(InOwnerCharacter);
	//
	//if (!!stateComponent)
	//	bCanMove ? stateComponent->SetMove() : stateComponent->SetStop();

	if (!!Montage)
	{
		InOwnerCharacter->PlayAnimMontage(Montage, PlayRatio);

		return;
	}

	//InOwnerCharacter->PlayAnimMontage(UCWeaponStructure::GetDefaultHitMontage(), PlayRatio);
}

void FDamageData::PauseMontage(class ACharacter* InOwnerCharacter, class UAnimInstance* InAnimInstance)
{
	//UCBehaviourComponent* behaviourComponent = CHelpers::GetComponent<UCBehaviourComponent>(InOwnerCharacter);

	//if (!!behaviourComponent)
	//{
	//	bCanMove ? behaviourComponent->SetMove() : behaviourComponent->SetStop();
	//	bFixedCamera ? behaviourComponent->EnableFixCamera() : behaviourComponent->DisableFixCamera();
	//}
	InAnimInstance->Montage_Pause(Montage);
}

void FDamageData::ResumeMontage(class ACharacter* InOwnerCharacter, class UAnimInstance* InAnimInstance)
{
	//UCBehaviourComponent* behaviourComponent = CHelpers::GetComponent<UCBehaviourComponent>(InOwnerCharacter);

	//if (!!behaviourComponent)
	//{
	//	bCanMove ? behaviourComponent->SetMove() : behaviourComponent->SetStop();
	//	bFixedCamera ? behaviourComponent->EnableFixCamera() : behaviourComponent->DisableFixCamera();
	//}
	InAnimInstance->Montage_Resume(Montage);
}

void FDamageData::AirborneSkill(class ACharacter* InAirborneCharacter)
{
	//FVector currentLoc = InAirborneCharacter->GetActorLocation();
	//FRotator currentRot = InAirborneCharacter->GetActorRotation();
	//USceneComponent* rootComponent = InAirborneCharacter->GetRootComponent();

	//FLatentActionInfo LatentInfo;
	//LatentInfo.CallbackTarget = InAirborneCharacter;
	//UKismetSystemLibrary::MoveComponentTo
	//(
	//	rootComponent, 
	//	FVector(currentLoc.X, currentLoc.Y, currentLoc.Z + 300.0f), 
	//	currentRot, 
	//	false, 
	//	false, 
	//	0.5f, 
	//	false, 
	//	EMoveComponentAction::Type::Move, 
	//	LatentInfo
	//);
	
	//InAirborneCharacter->SetActorRelativeLocation(FVector(currentLoc.X, currentLoc.Y, currentLoc.Z + 300.0f));
}
