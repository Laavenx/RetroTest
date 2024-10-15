// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_Launch.h"
#include <AIController.h>
#include "BehaviorTree/BlackboardComponent.h"
#include "RetroTest/Entities/RetroTestCharacterBase.h"

EBTNodeResult::Type UBTTask_Launch::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (AIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	APawn* OwnerPawn = AIController->GetPawn();

	if (!OwnerPawn)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BBComp = AIController->GetBlackboardComponent();
	if (!BBComp)
	{
		return EBTNodeResult::Failed;
	}
	
	if (auto TargetActor = Cast<ARetroTestCharacterBase>(BBComp->GetValueAsObject(BBTargetActor.SelectedKeyName)))
	{
		// WIP
		
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
