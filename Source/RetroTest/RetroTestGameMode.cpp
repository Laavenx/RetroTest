// Copyright Epic Games, Inc. All Rights Reserved.

#include "RetroTestGameMode.h"
#include "Player\RetroTestPlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARetroTestGameMode::ARetroTestGameMode()
{
	//
}

void ARetroTestGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	ConnectedControllers.Add(NewPlayer);

	//auto check = GetGameInstance()->IsDedicatedServerInstance();
}
