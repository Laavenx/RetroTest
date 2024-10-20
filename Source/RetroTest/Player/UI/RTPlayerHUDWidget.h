// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RTPlayerHUDWidget.generated.h"

class ARetroTestCharacterBase;
struct FGameplayEffectSpec;
struct FActiveGameplayEffectHandle;
class UAbilitySystemComponent;
struct FGameplayTag;
class URetroTestAbilitySystemComponent;
struct FOnAttributeChangeData;

UCLASS()
class RETROTEST_API URTPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCurrentHealth(float CurrentHealth);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCurrentCoins(float CurrentCoins);

	UFUNCTION()
	void OnGameplayEffectAdded(UAbilitySystemComponent* ASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle);

	void OnGameplayTagChanged(FGameplayTag InCooldownTag, int32 Count);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	TObjectPtr<class UTextBlock> CooldownMeter;

	FTimerHandle TimerHandle;

	TObjectPtr<URetroTestAbilitySystemComponent> AbilityComponent;

	virtual void HealthChanged(const FOnAttributeChangeData& Data);
	virtual void CoinsChanged(const FOnAttributeChangeData& Data);
};
