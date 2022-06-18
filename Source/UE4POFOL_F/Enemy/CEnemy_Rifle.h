/* MEMO: ACEnemy_Rifle.h 코드 배치 정리 완료  */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy/CEnemy.h"
#include "CEnemy_Rifle.generated.h"

UCLASS()
class UE4POFOL_F_API ACEnemy_Rifle : public ACEnemy
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	TSubclassOf<class ACProjectile> ProjectileClass;

	UPROPERTY()
	class ACProjectile* Projectile;

	UPROPERTY()
	class ACTriggerVolume_Spawner* TriggerVolumeSpanwer;

	//int32 Index;

public:
	ACEnemy_Rifle();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void SpawnAndShootProjectile();

	void OnFire();

	void GetMoveToLocation(FVector& OutLocation);

private:
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)override;

	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)override;

	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)override;

public:
	//FORCEINLINE ACSpline* GetSpline() { if (Splines[Index]) return Splines[Index]; else return nullptr; }
};
