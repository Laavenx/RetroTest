// Fill out your copyright notice in the Description page of Project Settings.


#include "RetroTestPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "UI/RTPlayerHUDWidget.h"

ARetroTestPlayerController::ARetroTestPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ARetroTestPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARetroTestPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARetroTestPlayerController::CreateHUD()
{
	if (UIMainHUD)
	{
		return;
	}
	
	if (!MainHUDClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing UIHUDWidgetClass. Please fill in on the Blueprint of the PlayerController."), *FString(__FUNCTION__));
		return;
	}

	// Only create a HUD for local player
	if (!IsLocalPlayerController())
	{
		return;
	}
	
	UIMainHUD = CreateWidget<URTPlayerHUDWidget>(this, MainHUDClass);
	UIMainHUD->AddToViewport();
}

void ARetroTestPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ARetroTestPlayerController::OnRep_PlayerState()
{
	// For edge cases where the PlayerState is repped before the Hero is possessed.
	Super::OnRep_PlayerState();
	CreateHUD();
}
