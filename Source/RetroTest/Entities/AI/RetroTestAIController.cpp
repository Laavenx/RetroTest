// Fill out your copyright notice in the Description page of Project Settings.

#include "RetroTestAIController.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "RetroTest/Entities/Data/RetroTestNPCConfig.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "RetroTest/Entities/NPC/RetroTestNPC.h"
#include "RetroTest/Player/RetroTestPlayerCharacter.h"


ARetroTestAIController::ARetroTestAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	AISenseSightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("SightSense");
	AISenseSightConfig->Implementation = UAISense_Sight::StaticClass();
	AISenseSightConfig->SightRadius = 800.0f;
	AISenseSightConfig->LoseSightRadius = 1000.0f;
	AISenseSightConfig->PeripheralVisionAngleDegrees = 180.0f;
	AISenseSightConfig->DetectionByAffiliation.bDetectEnemies = true;
	AISenseSightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	AISenseSightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	AISenseSightConfig->AutoSuccessRangeFromLastSeenLocation = -1.0;
	AISenseSightConfig->PointOfViewBackwardOffset = 0.0f;
	AISenseSightConfig->NearClippingRadius = 0.0f;
	AISenseSightConfig->SetMaxAge(15.0);
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));
	PerceptionComponent->ConfigureSense(*AISenseSightConfig);
	PerceptionComponent->SetDominantSense(UAISense_Sight::StaticClass());
}

void ARetroTestAIController::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(PerceptionComponent))
	{
		PerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &ARetroTestAIController::OnPerceptionUpdate);
		PerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ARetroTestAIController::OnTargetPerceptionForgotten);
	}
}

void ARetroTestAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	OwnerNPC = Cast<ARetroTestNPC>(GetPawn());
	SetupBlackboard();
}

void ARetroTestAIController::SetupBlackboard()
{
	if (OwnerNPC.IsValid())
	{
		if (OwnerNPC->UnitConfig)
		{
			RunBehaviorTree(OwnerNPC->UnitConfig->BehaviourTree);
			GetBlackboardComponent()->SetValueAsFloat("CombatRange", OwnerNPC->UnitConfig->CombatRange);
			AISenseSightConfig->SightRadius = OwnerNPC->UnitConfig->SightRadius;
			AISenseSightConfig->LoseSightRadius = OwnerNPC->UnitConfig->LoseSightRadius;
			GetAIPerceptionComponent()->ConfigureSense(*AISenseSightConfig);
		}
	}
}

void ARetroTestAIController::OnPerceptionUpdate(const TArray<AActor*>& UpdatedActors)
{
	for (AActor* Actor : UpdatedActors)
	{
		if (!IsValid(Actor)) { continue; }
		if (ARetroTestPlayerCharacter* Target = Cast<ARetroTestPlayerCharacter>(Actor))
		{
			GetBlackboardComponent()->SetValueAsObject("TargetActor", Target);
			break;
		}
	}
}

void ARetroTestAIController::OnTargetPerceptionForgotten(AActor* Actor)
{
	UE_LOG(LogTemp, Log, TEXT("Forgot target"))
}

void ARetroTestAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (OwnerNPC.IsValid())
	{
		const auto SelfLocation = OwnerNPC->GetActorLocation();
		if (SelfLocation != GetBlackboardComponent()->GetValueAsVector("SelfLocation"))
		{
			GetBlackboardComponent()->SetValueAsVector("SelfLocation", OwnerNPC->GetActorLocation());
		}
	}
}

