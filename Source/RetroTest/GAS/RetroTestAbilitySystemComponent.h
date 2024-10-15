// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "RetroTestAbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RETROTEST_API URetroTestAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	URetroTestAbilitySystemComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	bool GetCooldownForTag(FGameplayTagContainer CooldownTags, float& TimeRemaining, float& CooldownDuration) const;
};
