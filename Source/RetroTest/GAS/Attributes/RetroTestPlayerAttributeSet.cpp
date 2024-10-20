// Fill out your copyright notice in the Description page of Project Settings.


#include "RetroTestPlayerAttributeSet.h"

URetroTestPlayerAttributeSet::URetroTestPlayerAttributeSet()
{
}

void URetroTestPlayerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void URetroTestPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void URetroTestPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetCoinsAttribute())
	{
		NewValue = GetCoins() + NewValue;
	}
}

void URetroTestPlayerAttributeSet::OnRep_Coins(const FGameplayAttributeData& OldCoins)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URetroTestPlayerAttributeSet, Coins, OldCoins);
}
