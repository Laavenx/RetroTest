// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_AutoAttack.h"
#include <AIController.h>
#include "BehaviorTree/BlackboardComponent.h"
#include <AIController.h>

#include "RetroTest/GAS/RetroTestAbilitySystemComponent.h"
#include "RetroTest/Entities/NPC/RetroTestNPC.h"

EBTNodeResult::Type UBTTask_AutoAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	if (auto Character = Cast<ARetroTestNPC>(OwnerPawn))
	{
		auto ASC = Character->GetAbilitySystemComponent();
		
		// this can be used to block further activation
		if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Ability.AutoAttackActivation")))
		{
			return EBTNodeResult::Succeeded;
		}
		
		FGameplayAbilitySpec AbilitySpec = ASC->BuildAbilitySpecFromClass(GameplayAbility);
		ASC->GiveAbilityAndActivateOnce(AbilitySpec);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
