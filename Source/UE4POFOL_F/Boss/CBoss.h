/* MEMO: CBoss.h 코드 배치 정리 완료 */
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CBoss.generated.h"

UCLASS()
class UE4POFOL_F_API ACBoss : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Component")
	class UCBossComponent* BossComponent;

private:
	//UPROPERTY()
	//class UCapsuleComponent* Capsule;

	UPROPERTY()
	TSubclassOf<class UAnimInstance> AnimInstance;

	UPROPERTY()
	class ACharacter* Opponent;
	
	UPROPERTY()
	class UBlackboardComponent* Blackboard;
	
	UPROPERTY()
	class ACEnemy* Enemy[4];

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditDefaultsOnly, Category = "Attack Overall Setting")
	float AttackDelaySecond = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Attack Overall Setting")
	class UNiagaraSystem* LocationMarkEmitter;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attack Ball Setting")
	TSubclassOf<class ACBall> BallClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attack Ball Setting")
	TSubclassOf<class ACWeapon> GroundCrackWeaponClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attack Rocket Setting")
	TSubclassOf<class ACRocket> RocketClass;

	UPROPERTY(EditDefaultsOnly, Category = "Attack Call Friend Setting")
	TSubclassOf<class ACEnemy> CallFriendClass;

	FTimerHandle Timer;

	FHitResult HitResult;

	FVector EmitterLocation;

public:
	ACBoss();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void CreateLineTrace();

	void CheckLandingLocation();

	// DelaySecond 흐른 직 후에 한 번씩 호출되는 함수이다. Boss가 상황에 따른 Attack을 실시한다.  
	void Attack();
public:
	void ShootRocket();

	void ShootBall();

	void SpawnFriend();

	void SpawnNiagaraSystem();

	void MoveOpponentLoc();

	// 본 엑터가 소유한 무기의 오버랩 시작 시 호출된다. 이것은 DataAsset에서 weapon객체에 바인드 되어있다. 아래 UFUNCTION()이하도 마찬가지이다.
	UFUNCTION()
	void OwnedWeaponBeginOverlap(class ACharacter* InAttacker, class AActor* InAttackCauser, class ACharacter* InOtherCharacter);
	
	UFUNCTION()
	void OwnedWeaponEndOverlap(class ACharacter* InAttacker, class AActor* InAttackCauser, class ACharacter* InOtherCharacter);
	
	UFUNCTION()
	void OwnedWeaponOnCollision();
	
	UFUNCTION()
	void OwnedWeaponOffCollision();

private:
	UFUNCTION()
	void MontageEnded(UAnimMontage* InMontage, bool InInterrupted);

public:
	FORCEINLINE UBehaviorTree* GetBehaviorTree() { return BehaviorTree; }
	FORCEINLINE void SetBlackboard(UBlackboardComponent* InBlackboard) { Blackboard = InBlackboard; }
	FORCEINLINE ACharacter* GetOpponent() { return Opponent; }
};
