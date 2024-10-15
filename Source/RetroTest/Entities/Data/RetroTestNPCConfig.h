// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RetroTestNPCConfig.generated.h"

class UBehaviorTree;

UCLASS()
class RETROTEST_API URetroTestNPCConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CombatRange = 200;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float SightRadius = 800;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float LoseSightRadius = 1000;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UBehaviorTree> BehaviourTree = nullptr;
};
