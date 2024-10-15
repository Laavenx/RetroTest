// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "NPCAutoAttackAbility.generated.h"

class ARetroTestCharacterBase;
class ARetroTestNPC;
class URetroTestSettings;

UCLASS()
class RETROTEST_API UNPCAutoAttackAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* AutoAttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> AbilityEffect;

	TObjectPtr<ARetroTestNPC> AvatarActor;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AbilityCollision")
	bool bShouldHitMultipleTargets = false;

	/* How far we want to check for targets during hit */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AbilityCollision")
	float SweepRange = 100.f;

protected:
	UFUNCTION()
	void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	#if WITH_EDITORONLY_DATA
		UPROPERTY()
		const URetroTestSettings* DevSettings;
	#endif

	void SweepForTargets() const;
};
