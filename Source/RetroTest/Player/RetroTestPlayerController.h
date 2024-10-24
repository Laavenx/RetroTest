// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RetroTestPlayerController.generated.h"

class ARetroTestCharacterBase;
class ARetroTestPlayerCharacter;
class URTPlayerHUDWidget;
struct FOnAttributeChangeData;

UCLASS()
class RETROTEST_API ARetroTestPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ARetroTestPlayerController();

	void CreateHUD(ACharacter* PossesedChar);

protected:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void OnRep_PlayerState() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	TSubclassOf<URTPlayerHUDWidget> MainHUDClass;

	UPROPERTY()
	URTPlayerHUDWidget* UIMainHUD;

public:
	virtual void Tick(float DeltaTime) override;

	
};
