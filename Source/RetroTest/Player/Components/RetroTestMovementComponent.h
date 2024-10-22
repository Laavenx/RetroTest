// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RetroTestMovementComponent.generated.h"


class ARetroTestPlayerCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RETROTEST_API URetroTestMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	URetroTestMovementComponent();

public:
	// executes on client and server at the same time, which should result in more accurate results
	UFUNCTION(NetMulticast, Reliable)
	void Server_AddImpulse(FVector Vector);
	UFUNCTION(NetMulticast, Unreliable)
	void Server_ChangeGravity(float NewGravity);

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void NotifyJumpApex() override;
	virtual void Launch(FVector const& LaunchVel) override;

	virtual bool DoJump(bool bReplayingMoves) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement", meta = (DisplayName = "Jump Curve"))
	TObjectPtr<UCurveFloat> JumpCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement", meta = (DisplayName = "Fall Curve"))
	TObjectPtr<UCurveFloat> FallCurve;

public:
	void ProcessCustomJump(const float DeltaTime);
	void ProcessCustomFalling(const float DeltaTime);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement")
	float JumpForwardVelocityImpulseMult = 55;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement")
	float WalkingGravityScale = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement")
	float WalkingWalkSpeed = 600;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement")
	FRotator WalkingRotationRate = FRotator(0,450, 0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement")
	float FallingWalkSpeed = 1150;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Custom Movement")
	FRotator FallingRotationRate = FRotator(0,20,0);
	
	bool bIsLaunched;
	bool bIsJumping = false;
	float JumpDeltaTime;

	bool bIsFalling;
	float FallDeltaTime;
	
	FVector PrevCapsuleLocation;

private:
	TWeakObjectPtr<ARetroTestPlayerCharacter> RetroCharacter;
};
