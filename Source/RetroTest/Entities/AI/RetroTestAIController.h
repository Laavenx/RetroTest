// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "RetroTestAIController.generated.h"

class ARetroTestNPC;

UCLASS()
class RETROTEST_API ARetroTestAIController : public AAIController
{
	GENERATED_BODY()

public:
	ARetroTestAIController();

	virtual void OnPossess(APawn* InPawn) override;
	void SetupBlackboard();

	UFUNCTION()
	void OnPerceptionUpdate(const TArray<AActor*>& UpdatedActors);

	UFUNCTION()
	void OnTargetPerceptionForgotten(AActor* Actor);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UAISenseConfig_Sight> AISenseSightConfig;

	TWeakObjectPtr<ARetroTestNPC> OwnerNPC;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
