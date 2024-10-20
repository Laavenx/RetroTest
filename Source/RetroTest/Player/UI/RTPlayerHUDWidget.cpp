// Fill out your copyright notice in the Description page of Project Settings.


#include "RTPlayerHUDWidget.h"

#include "RetroTest/GAS/RetroTestAbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "RetroTest/GAS/Attributes/RetroTestPlayerAttributeSet.h"
#include "RetroTest/Player/RetroTestPlayerCharacter.h"

void URTPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	const auto PlayerCharacter = Cast<ARetroTestPlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	AbilityComponent = PlayerCharacter->FindComponentByClass<URetroTestAbilitySystemComponent>();
	
	const auto Attributes = Cast<URetroTestPlayerAttributeSet>(PlayerCharacter->GetAttributeSet());
	PlayerCharacter->GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(Attributes->GetHealthAttribute()).AddUObject(this, &URTPlayerHUDWidget::HealthChanged);
	PlayerCharacter->GetAbilitySystemComponent()->GetGameplayAttributeValueChangeDelegate(Attributes->GetCoinsAttribute()).AddUObject(this, &URTPlayerHUDWidget::CoinsChanged);

	AbilityComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &URTPlayerHUDWidget::OnGameplayEffectAdded);
	AbilityComponent->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag(FName(TEXT("Ability.SmashCooldown"))), EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &URTPlayerHUDWidget::OnGameplayTagChanged);
}

void URTPlayerHUDWidget::OnGameplayEffectAdded(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& Spec,
                                               FActiveGameplayEffectHandle Handle)
{
	// Player only has 1 ability
	FGameplayTagContainer GameplayContainer;
	GameplayContainer.AddTag(FGameplayTag::RequestGameplayTag(FName(TEXT("Ability.SmashCooldown"))));
	
	const FGameplayTag SmashCooldownTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Ability.SmashCooldown")));
	const auto CapturedSourceTags = Spec.CapturedSourceTags.GetSpecTags();
	if (!CapturedSourceTags.GetByIndex(0).MatchesTag(SmashCooldownTag))
	{
		return;
	}
	
	float TimeRemaining = 0;
	float CooldownDuration = 0;
	AbilityComponent->GetCooldownForTag(GameplayContainer,TimeRemaining, CooldownDuration);
	auto Text = FString::SanitizeFloat(CooldownDuration);
	CooldownMeter->SetText(FText::FromString(Text));
	if (TimeRemaining > 0.0f)
	{
		// Initiates timer which will decrease the cooldown number by 0.1
		FTimerDelegate Delegate;
		Delegate.BindLambda([=, this]() mutable
			{
				if (FMath::IsNegativeOrNegativeZero(TimeRemaining))
				{
					Text = FString::SanitizeFloat(0);
					CooldownMeter->SetText(FText::FromString(Text));
					GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
					return;
				}
				Text = FString::SanitizeFloat(TimeRemaining);
				CooldownMeter->SetText(FText::FromString(Text));
				TimeRemaining -= 0.1;
			});
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, Delegate, 0.1, true, 0);
	}
}

void URTPlayerHUDWidget::HealthChanged(const FOnAttributeChangeData& Data)
{
	// Update health bar
	SetCurrentHealth(Data.NewValue);
}

void URTPlayerHUDWidget::CoinsChanged(const FOnAttributeChangeData& Data)
{
	SetCurrentCoins(Data.NewValue);
}

void URTPlayerHUDWidget::OnGameplayTagChanged(FGameplayTag InCooldownTag, int32 Count)
{
	//
}
