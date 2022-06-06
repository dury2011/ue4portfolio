/* MEMO: CBTService_Boss.h 코드 배치 정리 완료 */
#include "AI/CBTService_Boss.h"
#include "Global.h"
#include "AI/CAIController_Boss.h"
#include "Component/CBossComponent.h"
#include "Boss/CBoss.h"

UCBTService_Boss::UCBTService_Boss()
{
	NodeName = "Service_Boss";
}

void UCBTService_Boss::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	ACAIController_Boss* bossController = Cast<ACAIController_Boss>(OwnerComp.GetOwner());
	ACBoss* boss = Cast<ACBoss>(bossController->GetPawn());
	UCBossComponent* bossComponent = CHelpers::GetComponent<UCBossComponent>(boss);
}