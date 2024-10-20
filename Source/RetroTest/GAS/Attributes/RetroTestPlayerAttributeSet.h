// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "RetroTestAttributeSetBase.h"
#include "RetroTestPlayerAttributeSet.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
UCLASS()
class RETROTEST_API URetroTestPlayerAttributeSet : public URetroTestAttributeSetBase
{
	GENERATED_BODY()

public:
	URetroTestPlayerAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	UPROPERTY(BlueprintReadOnly, Category = "Coins", ReplicatedUsing = OnRep_Coins)
	FGameplayAttributeData Coins;
	ATTRIBUTE_ACCESSORS(URetroTestPlayerAttributeSet, Coins);

protected:
	UFUNCTION()
	virtual void OnRep_Coins(const FGameplayAttributeData& OldCoins);
};
