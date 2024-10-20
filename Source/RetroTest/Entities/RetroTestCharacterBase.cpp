// Fill out your copyright notice in the Description page of Project Settings.


#include "RetroTestCharacterBase.h"

#include "RetroTest/GAS/RetroTestAbilitySystemComponent.h"
#include "RetroTest/GAS/Attributes/RetroTestAttributeSetBase.h"

ARetroTestCharacterBase::ARetroTestCharacterBase(const FObjectInitializer& object_initializer)
{
	PrimaryActorTick.bCanEverTick = true;

	AttributeSet = CreateDefaultSubobject<URetroTestAttributeSetBase>("AttributeSet");
	
	AbilitySystemComp = CreateDefaultSubobject<URetroTestAbilitySystemComponent>("RetroTestAbilityComponent");
	AbilitySystemComp->PrimaryComponentTick.bStartWithTickEnabled = false;
	AbilitySystemComp->SetIsReplicated(true);
	AbilitySystemComp->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	AbilitySystemComp->AddAttributeSetSubobject(AttributeSet.Get());
}

void ARetroTestCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// Can use GameplayEffectSpec to apply huge amount of attributes
	if (InitMaxHealth > 0)
	{
		GetAttributeSet()->SetMaxHealth(InitMaxHealth);
		GetAttributeSet()->SetHealth(InitMaxHealth);
	}
}

