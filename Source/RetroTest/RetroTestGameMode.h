// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RetroTestGameMode.generated.h"

UCLASS(minimalapi)
class ARetroTestGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARetroTestGameMode();

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

private:
	TArray<APlayerController*> ConnectedControllers;
};



