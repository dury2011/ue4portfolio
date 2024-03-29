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
	class ACTriggerVolume_Spawner* TriggerVolumeSpanwer;

public:
	ACEnemy_Rifle();

public:	
	virtual void Tick(float DeltaTime) override;

	void SpawnAndShootProjectile();

	virtual void OnAttack() override;

	void GetMoveToLocation(FVector& OutLocation);

	void TakeDamageAction_CannonRangedProjectile(float InDamage, ACProjectile* InProjectile);

private:
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)override;

	virtual void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)override;

	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)override;

protected:
	virtual void BeginPlay() override;

};
