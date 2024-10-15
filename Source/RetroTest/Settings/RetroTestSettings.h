// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "RetroTestSettings.generated.h"

UCLASS(Config=Game, defaultconfig, meta = (DisplayName="Debug Settings"))
class RETROTEST_API URetroTestSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "Gas|Collision")
	bool bDrawDebugGasCollision = false;
};
