// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_AutoAttack.generated.h"

class UGameplayAbility;

UCLASS()
class RETROTEST_API UBTTask_AutoAttack : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BBTargetActor;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayAbility> GameplayAbility;
};
