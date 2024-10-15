// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableFloorBase.generated.h"

class ARetroTestPlayerCharacter;

UCLASS()
class RETROTEST_API AInteractableFloorBase : public AActor
{
	GENERATED_BODY()

public:
	AInteractableFloorBase();

	virtual void TouchedMesh(AActor* Player);

protected:
	virtual void BeginPlay() override;

private:
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> FloorMesh;
};
