// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableFloorBase.h"
#include "..\..\Player\RetroTestPlayerCharacter.h"
#include "RetroTest/GAS/RetroTestAbilitySystemComponent.h"
#include "RetroTest/GAS/Attributes/RetroTestPlayerAttributeSet.h"


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
		URetroTestAbilitySystemComponent* OwnerAbilitySystem = Player->GetAbilitySystemComponent();
		FGameplayEffectContextHandle Context = OwnerAbilitySystem->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = OwnerAbilitySystem->MakeOutgoingSpec(AbilityEffect, 1, Context);

		if (!IsValid(OwnerAbilitySystem))
			return;
		
		OwnerAbilitySystem->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		// if (auto AttribSet = Cast<URetroTestPlayerAttributeSet>(Player->GetAttributeSet()))
		// {
		// 	AttribSet->SetDamage(99);
		// }
	}
}

