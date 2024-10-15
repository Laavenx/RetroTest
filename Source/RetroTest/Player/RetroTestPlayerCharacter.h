// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Logging/LogMacros.h"
#include "RetroTest/Entities/RetroTestCharacterBase.h"
#include "RetroTestPlayerCharacter.generated.h"

class USphereComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UGameplayAbility;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ARetroTestPlayerCharacter : public ARetroTestCharacterBase
{
	GENERATED_BODY()

public:
	ARetroTestPlayerCharacter(const FObjectInitializer& object_initializer);

	virtual void PossessedBy(AController* NewController) override;

private:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Shadow, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> BlobShadow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Shadow, meta = (AllowPrivateAccess = "true"))
	float BlobDrawDistance = 2500;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Shadow, meta = (AllowPrivateAccess = "true"))
	UMaterialParameterCollection* BlobMaterialInstance;

protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SmashAction;

protected:
	UFUNCTION(Server, Reliable)
	void Server_Smash();

	virtual void OnRep_PlayerState() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayAbility> SmashAbility;
	FGameplayAbilitySpecHandle SmashAbilityHandle;

private:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void OnWalkingOffLedge_Implementation(const FVector& PreviousFloorImpactNormal,
		const FVector& PreviousFloorContactNormal, const FVector& PreviousLocation, float TimeDelta) override;

private:
	
	bool CanCharacterJump();
	virtual void CheckJumpInput(float DeltaTime) override;

	FTimerHandle JumpTimerHandle;
	bool bShouldPredictJump;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float JumpPredictionTime = 0.15;

public:
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	bool GetShouldPredictJump() const { return bShouldPredictJump; }
	void SetShouldPredictJump(const bool bShouldJump) { bShouldPredictJump = bShouldJump; }

	FVector3d CurrentSpawnLocation;
	
};
