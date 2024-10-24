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

	// I don't know how to properly replicate this
	// Replicating the ConstrainedMesh results in jittery movement, probably should look into new physics prediction feature
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
	
	// We only want to affect player
	if (const auto Player = Cast<ARetroTestPlayerCharacter>(Other))
	{
		// Player affects this physics object with this velocity
		//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + Player->GetMovementComponent()->Velocity, FColor::Green, false, 1.5, 0, 2.f);
		// Hit direction from center of other actor towards ConstrainedMesh
		//DrawDebugLine(GetWorld(), Other->GetActorLocation(), Other->GetActorLocation() + (HitNormal * 100), FColor::Purple, false, 1.5);

		// Vector the mesh is pushing towards
		const auto ConstrainedMeshVelocity = ConstrainedMesh->GetComponentVelocity();

		// When player is standing still, push
		if (Player->GetMovementComponent()->Velocity.Size() == 0)
		{
			const FVector ReverseVector = FVector(-ConstrainedMeshVelocity.X, -ConstrainedMeshVelocity.Y, -ConstrainedMeshVelocity.Z);
			const auto ForceToPushWith = ReverseVector * ConstrainedMesh->GetMass() * PushForceMultiplier;
			Player->GetRetroTestCharacterMovement()->Server_AddImpulse(ForceToPushWith);
			return;
		}
		
		const double DotProduct = UKismetMathLibrary::Dot_VectorVector(ConstrainedMeshVelocity, Player->GetMovementComponent()->Velocity);
		// If both actors are pushing in the same direction, ignore player
		if (DotProduct >= 0)
		{
			UE_LOG(LogTemp, Log, TEXT("Meshes are pushing in the same direction %f"), DotProduct);
		}
		else if (DotProduct < 0)
		{
			const FVector ReverseVector = FVector(-ConstrainedMeshVelocity.X, -ConstrainedMeshVelocity.Y, -ConstrainedMeshVelocity.Z);
			const auto ForceToPushWith = ReverseVector * ConstrainedMesh->GetMass() * PushForceMultiplier;
			// Push player
			Player->GetRetroTestCharacterMovement()->Server_AddImpulse(ForceToPushWith);
			UE_LOG(LogTemp, Log, TEXT("Meshes pushing in opposite direction %f"), DotProduct);
		}
	}
}

