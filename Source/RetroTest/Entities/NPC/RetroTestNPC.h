// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RetroTest/Entities/RetroTestCharacterBase.h"
#include "RetroTestNPC.generated.h"

UCLASS()
class RETROTEST_API ARetroTestNPC : public ARetroTestCharacterBase
{
	GENERATED_BODY()

public:
	ARetroTestNPC(const FObjectInitializer& object_initializer);

	void SetBlackBoardShouldExecute(const bool bShouldExecute) const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<class URetroTestNPCConfig> UnitConfig;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
