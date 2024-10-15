// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsWithinCombatRange.h"
#include <AIController.h>
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "RetroTest/Entities/Data/RetroTestNPCConfig.h"
#include "RetroTest/Entities/NPC/RetroTestNPC.h"

bool UBTDecorator_IsWithinCombatRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	APawn* OwnerPawn = AIController->GetPawn();
	if (OwnerPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BBComp = AIController->GetBlackboardComponent();
	if (!BBComp)
	{
		return EBTNodeResult::Failed;
	}

	if (auto Character = Cast<ARetroTestNPC>(OwnerPawn))
	{
		AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject(BBTargetActor.SelectedKeyName));
		if (!IsValid(TargetActor))
		{
			return EBTNodeResult::Failed;
		} 

		FVector ActorLocation = TargetActor->GetActorLocation();
		FVector OwnerLocation = OwnerPawn->GetActorLocation();
		float DistanceFromActor = UKismetMathLibrary::VSizeXY(OwnerLocation - ActorLocation);
		float CombatRange = Character->UnitConfig->CombatRange;

		return DistanceFromActor <= CombatRange;
	}

	return EBTNodeResult::Failed;
}
