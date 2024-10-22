// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicsInteractable.h"

#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "RetroTest/Player/RetroTestPlayerCharacter.h"

APhysicsInteractable::APhysicsInteractable()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	ConstrainedMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ConstrainedMesh"));
	ConstrainedMesh->SetupAttachment(GetRootComponent());
	ConstrainedMesh->SetSimulatePhysics(true);
}

void APhysicsInteractable::BeginPlay()
{
	Super::BeginPlay();
	
}

void APhysicsInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APhysicsInteractable::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// I don't know how to properly replicate this
	// Replicating the ConstrainedMesh results in jittery movement
	// We only want to affect player
	if (auto Player = Cast<ARetroTestPlayerCharacter>(Other))
	{
		//UE_LOG(LogTemp, Log, TEXT("Velocity of Player is: %f hinge: %f"), Player->GetMovementComponent()->Velocity.Size(), ConstrainedMesh->GetComponentVelocity().Size());

		// Player affects this physics object with this velocity
		DrawDebugLine(GetWorld(), Other->GetActorLocation(), Other->GetActorLocation() + Player->GetMovementComponent()->Velocity, FColor::Green, false, 1.5, 0, 2.f);
		// Hit direction from center of other actor towards ConstrainedMesh
		//DrawDebugLine(GetWorld(), Other->GetActorLocation(), Other->GetActorLocation() + (HitNormal * 100), FColor::Purple, false, 1.5);

		// Check if player is pushing the hinge
		if (Player->GetMovementComponent()->Velocity.Size() < 1)
		{
			const auto ConstrainedMeshVelocity = ConstrainedMesh->GetComponentVelocity();
			const FVector ReverseVector = FVector(-ConstrainedMeshVelocity.X, -ConstrainedMeshVelocity.Y, -ConstrainedMeshVelocity.Z);
			const auto ForceToPushWith = ReverseVector * ConstrainedMesh->GetMass() * PushForceMultiplier;
			Player->GetRetroTestCharacterMovement()->Server_AddImpulse(ForceToPushWith);
			//DrawDebugLine(GetWorld(), Player->GetActorLocation(), Player->GetActorLocation() + ReverseVector, FColor::Blue, false, 1.5, 0, 2.5f);
		}
		
		auto Forward = ConstrainedMesh->GetForwardVector();
		auto DotProduct = UKismetMathLibrary::Dot_VectorVector(Forward, Player->GetMovementComponent()->Velocity);

		UE_LOG(LogTemp, Log, TEXT("DOTPRODUCT IS EQUAL: %f"), DotProduct);
		if (DotProduct > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("<90 degrees"));
		}
		else if (DotProduct < 0)
		{
			UE_LOG(LogTemp, Log, TEXT(">90 degrees"));
		}
		else
		{
			//
			UE_LOG(LogTemp, Log, TEXT("90 degree hit"));
		}
	}
}

