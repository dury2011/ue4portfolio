/* MEMO: CSwordAura.h 코드 배치 정리 완료 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraDataInterfaceExport.h"
#include "CSwordAura.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnBoxOverlap, class ACharacter*, InAttacker, class AActor*, InAttackCauser, class ACharacter*, InOtherCharacter);

UCLASS()
class UE4POFOL_F_API ACSwordAura : public AActor, public INiagaraParticleCallbackHandler
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnBoxOverlap OnSwordAuraBoxBeginOverlap;

	UPROPERTY(BlueprintAssignable)
	FOnBoxOverlap OnSwordAuraBoxEndOverlap;

private:
	UPROPERTY(VisibleDefaultsOnly)
	class USceneComponent* Root;

	UPROPERTY(VisibleDefaultsOnly)
	class UNiagaraComponent* Niagara;

	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* Box;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageInterval = 0.1f;

	FVector InitialBoxScale;
	FTimerHandle HitTimerHandle;
	TArray<class ACharacter*> Hitted;
	
public:	
	ACSwordAura();

protected:
	virtual void BeginPlay() override;

public:
	virtual void BeginDestroy() override;
	virtual void Tick(float DeltaTime) override;
	void ReceiveParticleData_Implementation(const TArray<FBasicParticleData>& Data, UNiagaraSystem* NiagaraSystem);

private:
	UFUNCTION()
	void OnSystemFinished(class UNiagaraComponent* PSystem);

public:
	void ToggleOn();
	void ToggleOff();

private:
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
