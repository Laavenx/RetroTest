// Fill out your copyright notice in the Description page of Project Settings.


#include "RetroTestAbility.h"

#include "RetroTest/GAS/RetroTestAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "RetroTest/Entities/RetroTestCharacterBase.h"
#include "RetroTest/Settings/RetroTestSettings.h"

void URetroTestAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	DevSettings = GetDefault<URetroTestSettings>();
	AvatarActor = Cast<ARetroTestCharacterBase>(GetAvatarActorFromActorInfo());
	ExecuteAbility();
}

void URetroTestAbility::ExecuteAbility()
{
	if (!IsValid(AvatarActor))
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
		return;
	}

	const FCollisionShape CapsuleShape = AvatarActor->GetCapsuleComponent()->GetCollisionShape();
	const auto Rotation = AvatarActor->GetActorRotation().Quaternion();

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(AvatarActor);
	
	// Sweep in front
	const FVector SweepDestination = AvatarActor->GetActorLocation() + (AvatarActor->GetActorForwardVector() * SweepRange);
	
	bool bIsBlockingHit;
	TArray<FHitResult> OutHits;
	if (bShouldHitMultipleTargets)
	{
		bIsBlockingHit = GetWorld()->SweepMultiByProfile(OutHits, AvatarActor->GetActorLocation(), SweepDestination,
			Rotation, AvatarActor->GetCapsuleComponent()->GetCollisionProfileName(), CapsuleShape, CollisionParams);
	} else
	{
		FHitResult OutHit;
		bIsBlockingHit = GetWorld()->SweepSingleByProfile(OutHit, AvatarActor->GetActorLocation(), SweepDestination,
			 Rotation, AvatarActor->GetCapsuleComponent()->GetCollisionProfileName(), CapsuleShape, CollisionParams);
		OutHits.Add(OutHit);
	}
	
	if (DevSettings->bDrawDebugGasCollision)
	{
		DrawDebugCapsule(GetWorld(), AvatarActor->GetActorLocation(), CapsuleShape.GetCapsuleHalfHeight(),
			CapsuleShape.GetCapsuleRadius(), Rotation, FColor::Blue, false, 5, 0, 1.f);
		DrawDebugCapsule(GetWorld(), SweepDestination, CapsuleShape.GetCapsuleHalfHeight(),
			CapsuleShape.GetCapsuleRadius(), Rotation, FColor::Red, false, 5, 0, 1.f);
	}
	
	if (bIsBlockingHit)
	{
		for (auto Hit : OutHits)
		{
			if (const auto HitActor = Cast<ARetroTestCharacterBase>(Hit.GetActor()))
			{
				URetroTestAbilitySystemComponent* OwnerAbilitySystem = HitActor->GetAbilitySystemComponent();
				FGameplayEffectContextHandle Context = OwnerAbilitySystem->MakeEffectContext();
				FGameplayEffectSpecHandle SpecHandle = OwnerAbilitySystem->MakeOutgoingSpec(AbilityEffect, 1, Context);

				TArray<TWeakObjectPtr<AActor>> TargetActors;
				TargetActors.Add(HitActor);
				Context.AddActors(TargetActors);

				URetroTestAbilitySystemComponent* TargetAbilitySystem = HitActor->FindComponentByClass<URetroTestAbilitySystemComponent>();

				if (!IsValid(TargetAbilitySystem))
					return;
				
				OwnerAbilitySystem->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetAbilitySystem);
			}
		}
	}
	
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}
