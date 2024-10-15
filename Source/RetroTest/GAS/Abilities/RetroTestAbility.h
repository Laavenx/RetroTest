// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "RetroTestAbility.generated.h"

class ARetroTestCharacterBase;
class URetroTestSettings;

UCLASS()
class RETROTEST_API URetroTestAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void ExecuteAbility();
	
	TObjectPtr<ARetroTestCharacterBase> AvatarActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AbilityCollision")
	bool bShouldHitMultipleTargets = false;

	/* How far we want to check for targets during hit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityCollision")
	float SweepRange = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> AbilityEffect;


protected:
	#if WITH_EDITORONLY_DATA
		UPROPERTY()
		const URetroTestSettings* DevSettings;
	#endif
};
