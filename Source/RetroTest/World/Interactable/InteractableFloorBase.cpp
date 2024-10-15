// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableFloorBase.h"
#include "..\..\Player\RetroTestPlayerCharacter.h"


AInteractableFloorBase::AInteractableFloorBase()
{
	PrimaryActorTick.bCanEverTick = true;

	FloorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMesh"));
	SetRootComponent(FloorMesh);
	FloorMesh->SetCastShadow(false);
	FloorMesh->SetComponentTickEnabled(false);
	FloorMesh->SetGenerateOverlapEvents(false);
}

void AInteractableFloorBase::BeginPlay()
{
	Super::BeginPlay();
}

void AInteractableFloorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractableFloorBase::TouchedMesh(AActor* Actor)
{
	if (auto Player = Cast<ARetroTestPlayerCharacter>(Actor))
	{
		FVector3d LocationToUse = Player->CurrentSpawnLocation;
		//Player->Destroy();

		Player->SetActorLocation(Player->CurrentSpawnLocation);
		// FActorSpawnParameters SpawnInfo;
		// FRotator Rotation(0.0f, 0.0f, 0.0f);
		// auto SpawnedActor = GetWorld()->SpawnActor<ARetroTestPlayerCharacter>(LocationToUse, Rotation, SpawnInfo);
		// GetWorld()->GetFirstPlayerController()->Possess(SpawnedActor);
	}
}

