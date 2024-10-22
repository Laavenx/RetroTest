// Fill out your copyright notice in the Description page of Project Settings.

#include "RetroTestMovementComponent.h"

#include "KismetTraceUtils.h"
#include "..\RetroTestPlayerCharacter.h"
#include "GameFramework/Character.h"
#include "Components/capsuleComponent.h"
#include "RetroTest/World/Interactable/InteractableFloorBase.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"

URetroTestMovementComponent::URetroTestMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// TODO: probably need a custom function
	bUseFlatBaseForFloorChecks = true;
}

void URetroTestMovementComponent::Server_ChangeGravity_Implementation(float NewGravity)
{
	GravityScale = NewGravity;
}

void URetroTestMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	RetroCharacter = Cast<ARetroTestPlayerCharacter>(CharacterOwner);
}

void URetroTestMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                 FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ProcessCustomJump(DeltaTime);
	ProcessCustomFalling(DeltaTime);

	const FVector CapsuleLocation = UpdatedComponent->GetComponentLocation();
	FVector PlayerMovementDirection = CapsuleLocation - PrevCapsuleLocation;
	PlayerMovementDirection.Normalize();
	PlayerMovementDirection *= 10;

	auto NormalizedVelocity = Velocity;
	NormalizedVelocity.Normalize();
	NormalizedVelocity *= 150;
	
	const FVector LineTracedDestination = CapsuleLocation - FVector(0,0,26.75) + NormalizedVelocity;

	FCollisionQueryParams CollisionParameters = FCollisionQueryParams::DefaultQueryParam;
	CollisionParameters.AddIgnoredActor(CharacterOwner);
	
	FCollisionObjectQueryParams ObjectsToHit;
	ObjectsToHit.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectsToHit.AddObjectTypesToQuery(ECC_WorldDynamic);
	
	FHitResult TraceOutHit;
	
	bool bIsBlockingHit = GetWorld()->LineTraceSingleByObjectType(TraceOutHit, CapsuleLocation - FVector(0,0,26.75), LineTracedDestination,
		ObjectsToHit, CollisionParameters);
	
	if (bIsBlockingHit)
	{
		// Detect touching lava floors and similar
		if (auto Floor = Cast<AInteractableFloorBase>(TraceOutHit.GetActor()))
		{
			Floor->TouchedMesh(GetOwner());
		}
		
		// Experimental code for making the slime be able to bounce off of walls
		// DrawDebugSphere(GetWorld(), TraceOutHit.ImpactPoint, 5.f, 16, FColor::Orange, false, 1.5, 0, 1.f);
		// const FVector LaunchVector = FMath::GetReflectionVector(Velocity, TraceOutHit.ImpactNormal);
		// if (bIsJumping)
		// {
		// 	auto ZClamp = FMath::Clamp(LaunchVector.Z, -9000.0, 9000.0);
		// 	Launch(FVector(LaunchVector.X, LaunchVector.Y, LaunchVector.Z) * 1.25);
		// }
	}
	
	// fixes being pushed by other objects
	// FHitResult OutHit;
	// SafeMoveUpdatedComponent(FVector(0.f, 0.f, 0.01f), CharacterOwner->GetActorRotation(), true, OutHit);
	// SafeMoveUpdatedComponent(FVector(0.f, 0.f, -0.01f), CharacterOwner->GetActorRotation(), true, OutHit);

	PrevCapsuleLocation = UpdatedComponent->GetComponentLocation();
}

void URetroTestMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	// Reset variables, set custom movement per movement mode
	if (MovementMode == MOVE_Falling)
	{
		bNotifyApex = true;
		MaxWalkSpeed = FallingWalkSpeed;
		RotationRate = FallingRotationRate;
		FallDeltaTime = 0.0f;

		UE_LOG(LogTemp, Warning, TEXT("Falling"));
	}

	if (MovementMode == MOVE_Flying)
	{
		UE_LOG(LogTemp, Warning, TEXT("Flying"));
	}

	if (MovementMode == MOVE_Walking)
	{
		MaxWalkSpeed = WalkingWalkSpeed;
		Server_ChangeGravity(WalkingGravityScale);
		RotationRate = WalkingRotationRate;
		JumpDeltaTime = 0.0f;

		if (RetroCharacter.IsValid() && RetroCharacter->GetShouldPredictJump())
		{
			DoJump(RetroCharacter->bClientUpdating);
			RetroCharacter->SetShouldPredictJump(false);
		}

		UE_LOG(LogTemp, Warning, TEXT("Walking"));
	}
}

void URetroTestMovementComponent::NotifyJumpApex()
{
	Super::NotifyJumpApex();
	
	bIsJumping = false;
	bIsFalling = true;
	UE_LOG(LogTemp, Warning, TEXT("Reached apex"));
}

void URetroTestMovementComponent::Launch(FVector const& LaunchVel)
{
	Super::Launch(LaunchVel);
	bIsLaunched = true;
}

bool URetroTestMovementComponent::DoJump(bool bReplayingMoves)
{
	if (Super::DoJump(bReplayingMoves))
	{
		bIsJumping = true;
		bIsFalling = false;
		
		Server_ChangeGravity(2);
		
		// Increase forward jump length
		const auto ForwardVector = FVector(Velocity.X, Velocity.Y, 0.0) * JumpForwardVelocityImpulseMult;
		Server_AddImpulse(ForwardVector);
		return true;
	}

	return false;
}

void URetroTestMovementComponent::Server_AddImpulse_Implementation(FVector Vector)
{
	AddImpulse(Vector);
}

void URetroTestMovementComponent::ProcessCustomJump(const float DeltaTime)
{
	if (bIsJumping && JumpCurve)
	{
		JumpDeltaTime += DeltaTime;
		const float JumpCurveValue = JumpCurve->GetFloatValue(JumpDeltaTime);

		Server_ChangeGravity(JumpCurveValue);
	}
}

void URetroTestMovementComponent::ProcessCustomFalling(const float DeltaTime)
{
	if (bIsFalling && FallCurve)
	{
		FallDeltaTime += DeltaTime;
		const float FallCurveValue = FallCurve->GetFloatValue(FallDeltaTime);
		
		Server_ChangeGravity(FallCurveValue);
	}
}
