// Fill out your copyright notice in the Description page of Project Settings.


#include "RetroTestAttributeSetBase.h"

#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "Kismet/KismetMathLibrary.h"
#include "RetroTest/Player/RetroTestPlayerCharacter.h"
#include "RetroTest/Player/Components/RetroTestMovementComponent.h"

URetroTestAttributeSetBase::URetroTestAttributeSetBase()
	: Health(1.f)
	, MaxHealth(1.f)
	, KnockBackFactor(0.f)
{
}

void URetroTestAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(URetroTestAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(URetroTestAttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);
}

void URetroTestAttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
}

void URetroTestAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float DamageDone = GetDamage();
		SetDamage(0.f);
		if (DamageDone >= 0.0f)
		{
			const float NewHealth = GetHealth() - DamageDone;
			SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));

			if (NewHealth <= 0)
			{
				const auto TargetActor = Cast<ARetroTestCharacterBase>(Data.Target.AbilityActorInfo->AvatarActor.Get());
				TargetActor->Die();
			}
		}
	}

	if (Data.EvaluatedData.Attribute == GetKnockBackFactorAttribute())
	{
		const auto KnockBackFactorToApply = GetKnockBackFactor();
		SetKnockBackFactor(0.f);
		if (IsValid(Data.Target.GetAvatarActor()))
		{
			const ARetroTestPlayerCharacter* Player = Cast<ARetroTestPlayerCharacter>(Data.Target.GetAvatarActor());
			const FHitResult* Hit = Data.EffectSpec.GetContext().GetHitResult();

			// for the purpose of this game, only player gets knockback currently
			if (Player && Hit)
			{
				const auto MovementComponent = Cast<URetroTestMovementComponent>(Player->GetMovementComponent());
				// Reverse vector from the ImpactNormal (point at which the shapes overlapped)
				const auto ReveredVector = FVector3d(-Hit->ImpactNormal.X, -Hit->ImpactNormal.Y, -Hit->ImpactNormal.Z) * KnockBackFactorToApply;
				MovementComponent->Server_AddImpulse(ReveredVector);
			}
		}
	}
}

void URetroTestAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URetroTestAttributeSetBase, Health, OldHealth);
}

void URetroTestAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(URetroTestAttributeSetBase, MaxHealth, OldMaxHealth);
}
