// Copyright Epic Games, Inc. All Rights Reserved.

#include "RetroTestGameMode.h"
#include "Player\RetroTestPlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARetroTestGameMode::ARetroTestGameMode()
{
	//
}

void ARetroTestGameMode::PlayerDied(AController* Controller)
{
	FTimerHandle RespawnTimerHandle;
	FTimerDelegate RespawnDelegate;

	RespawnDelegate = FTimerDelegate::CreateUObject(this, &ARetroTestGameMode::RespawnPlayer, Controller);
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnDelay, false);
}

void ARetroTestGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	ConnectedControllers.Add(NewPlayer);

	//auto check = GetGameInstance()->IsDedicatedServerInstance();
}

void ARetroTestGameMode::RespawnPlayer(AController* Controller)
{
	if (Controller->IsPlayerController())
	{
		// Respawn player
		const AActor* PlayerStart = FindPlayerStart(Controller);

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		
		ARetroTestPlayerCharacter* Player = GetWorld()->SpawnActor<ARetroTestPlayerCharacter>(DefaultPawnClass, PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation(), SpawnParameters);

		// Destroy old pawn if it exists
		APawn* OldSpectatorPawn = Controller->GetPawn();
		if (IsValid(OldSpectatorPawn))
		{
			Controller->UnPossess();
			OldSpectatorPawn->Destroy();
		}
		Controller->Possess(Player);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ARetroTestGameMode::RespawnPlayer::There is not respawn functionality for AI"))
	}
}
