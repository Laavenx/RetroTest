// Copyright Epic Games, Inc. All Rights Reserved.

#include "RetroTestPlayerCharacter.h"

#include "RetroTest/GAS/RetroTestAbilitySystemComponent.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "RetroTestPlayerController.h"
#include "RetroTest/RetroTestGameMode.h"
#include "RetroTest/GAS/Attributes/RetroTestPlayerAttributeSet.h"
#include "RetroTest/Player/Components/RetroTestMovementComponent.h"
#include "RetroTest/Entities/Components/BlobShadowComponent.h"
#include "RetroTest/World/Interactable/PhysicsInteractable.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ARetroTestPlayerCharacter::ARetroTestPlayerCharacter(const FObjectInitializer& object_initializer)
	: Super(object_initializer.SetDefaultSubobjectClass<URetroTestMovementComponent>(CharacterMovementComponentName)
	.SetDefaultSubobjectClass<URetroTestPlayerAttributeSet>("AttributeSet"))
{
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate
	
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false;

	BlobShadowComponent = CreateDefaultSubobject<UBlobShadowComponent>(TEXT("BlobShadowComponent"));
	BlobShadowComponent->SetupAttachment(RootComponent);
}

// Server
void ARetroTestPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ARetroTestPlayerController* PlayerController = Cast<ARetroTestPlayerController>(Controller);
	if (PlayerController)
	{
		PlayerController->CreateHUD(this);
	}
}

// Both client and server will call this function
void ARetroTestPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentSpawnLocation = GetActorLocation();
	
	if (ARetroTestPlayerController* PlayerController = Cast<ARetroTestPlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Register ability on the server
	if (HasAuthority())
	{
		if (SmashAbility && AbilitySystemComp)
		{
			SmashAbilityHandle = AbilitySystemComp->GiveAbility(SmashAbility);
		}
	}
}

void ARetroTestPlayerCharacter::Destroyed()
{
	Super::Destroyed();
}

void ARetroTestPlayerCharacter::Die()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0;
	GetCharacterMovement()->Velocity = FVector(0);

	if (IsValid(AbilitySystemComp))
	{
		AbilitySystemComp->CancelAllAbilities();
	}

	// if (DeathMontage)
	// {
	// 	PlayAnimMontage(DeathMontage);
	// }

	if (HasAuthority())
	{
		ARetroTestGameMode* GM = Cast<ARetroTestGameMode>(GetWorld()->GetAuthGameMode());

		if (GM)
		{
			GM->PlayerDied(GetController());
		}
	}
	
	Destroy();
}

void ARetroTestPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ARetroTestPlayerCharacter::OnWalkingOffLedge_Implementation(const FVector& PreviousFloorImpactNormal,
	const FVector& PreviousFloorContactNormal, const FVector& PreviousLocation, float TimeDelta)
{
	UE_LOG(LogTemplateCharacter, Log, TEXT("Walked off of ledge"));
}

// Had to create a new function because CanJump() was const
bool ARetroTestPlayerCharacter::CanCharacterJump()
{
	// Ensure that the CharacterMovement state is valid
	bool bJumpIsAllowed = GetCharacterMovement()->CanAttemptJump();

	if (bJumpIsAllowed)
	{
		// Ensure JumpHoldTime and JumpCount are valid.
		if (!bWasJumping || GetJumpMaxHoldTime() <= 0.0f)
		{
			if (JumpCurrentCount == 0 && GetCharacterMovement()->IsFalling())
			{
				bJumpIsAllowed = JumpCurrentCount + 1 < JumpMaxCount;
			}
			else
			{
				bJumpIsAllowed = JumpCurrentCount < JumpMaxCount;
			}
		}
		else
		{
			// Only consider JumpKeyHoldTime as long as:
			// A) The jump limit hasn't been met OR
			// B) The jump limit has been met AND we were already jumping
			const bool bJumpKeyHeld = (bPressedJump && JumpKeyHoldTime < GetJumpMaxHoldTime());
			bJumpIsAllowed = bJumpKeyHeld &&
				((JumpCurrentCount < JumpMaxCount) || (bWasJumping && JumpCurrentCount == JumpMaxCount));
		}
	}
	
	GetWorld()->GetTimerManager().ClearTimer(JumpTimerHandle);

	// initiate timer which will allow the player to automatically jump within a window when landing
	if (!bJumpIsAllowed)
	{
		SetShouldPredictJump(true);
		FTimerDelegate Delegate{
			FTimerDelegate::CreateUObject(this, &ARetroTestPlayerCharacter::SetShouldPredictJump, false)
		};
		
		GetWorld()->GetTimerManager().SetTimer(JumpTimerHandle, Delegate, 0.25, false);
	}

	return bJumpIsAllowed;
}

void ARetroTestPlayerCharacter::CheckJumpInput(float DeltaTime)
{
	JumpCurrentCountPreJump = JumpCurrentCount;

	if (GetCharacterMovement())
	{
		if (bPressedJump)
		{
			// If this is the first jump and we're already falling,
			// then increment the JumpCount to compensate.
			const bool bFirstJump = JumpCurrentCount == 0;
			if (bFirstJump && GetCharacterMovement()->IsFalling())
			{
				JumpCurrentCount++;
			}

			const bool bDidJump = CanCharacterJump() && GetCharacterMovement()->DoJump(bClientUpdating);
			if (bDidJump)
			{
				// Transition from not (actively) jumping to jumping.
				if (!bWasJumping)
				{
					JumpCurrentCount++;
					JumpForceTimeRemaining = GetJumpMaxHoldTime();
					OnJumped();
				}
			}

			bWasJumping = bDidJump;
		}
	}
}

void ARetroTestPlayerCharacter::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (auto Interactable = Cast<APhysicsInteractable>(Other))
	{
		//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + (Hit.ImpactNormal* 100), FColor::Red, false, 1.5);
		//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + (HitNormal * 100), FColor::Green, false, 1.5);
		//DrawDebugLine(GetWorld(), GetActorLocation(), Hit., FColor::Green, false, 1.5);
	}
}

// Client
void ARetroTestPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	ARetroTestPlayerController* PlayerController = Cast<ARetroTestPlayerController>(Controller);
	if (PlayerController)
	{
		PlayerController->CreateHUD(this);
	}
}

void ARetroTestPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARetroTestPlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARetroTestPlayerCharacter::Look);
		EnhancedInputComponent->BindAction(SmashAction, ETriggerEvent::Triggered, this, &ARetroTestPlayerCharacter::Server_Smash);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ARetroTestPlayerCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (IsValid(Controller))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ARetroTestPlayerCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (IsValid(Controller))
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ARetroTestPlayerCharacter::Server_Smash_Implementation()
{
	if (!IsValid(SmashAbility))
	{
		return;
	}
	
	AbilitySystemComp->TryActivateAbility(SmashAbilityHandle);
}
