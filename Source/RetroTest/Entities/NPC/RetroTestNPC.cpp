// Fill out your copyright notice in the Description page of Project Settings.


#include "RetroTestNPC.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "RetroTest/Entities/AI/RetroTestAIController.h"


ARetroTestNPC::ARetroTestNPC(const FObjectInitializer& object_initializer) : Super(object_initializer)
{
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = ARetroTestAIController::StaticClass();
}

void ARetroTestNPC::SetBlackBoardShouldExecute(const bool bShouldExecute) const
{
	const auto AIController = Cast<ARetroTestAIController>(GetController());
	if (IsValid(AIController))
	{
		AIController->GetBlackboardComponent()->SetValueAsBool("bShouldExecute", bShouldExecute);
	}
}

void ARetroTestNPC::BeginPlay()
{
	Super::BeginPlay();
}

void ARetroTestNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

