// Fill out your copyright notice in the Description page of Project Settings.


#include "NPCAutoAttackAbility.h"
#include "RetroTest/GAS/RetroTestAbilitySystemComponent.h"
#include "Components/capsuleComponent.h"
#include "RetroTest/Entities/RetroTestCharacterBase.h"
#include "RetroTest/Entities/NPC/RetroTestNPC.h"
#include "RetroTest/GAS/AbilityTasks/RTAT_PLayMontageAndWaitForEvent.h"
#include "RetroTest/Player/RetroTestPlayerCharacter.h"
#include "RetroTest/Settings/RetroTestSettings.h"

void UNPCAutoAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	if (!IsValid(AutoAttackMontage))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	// Get npc actor and disable behaviour tree for the duration of montage
	AvatarActor = Cast<ARetroTestNPC>(GetAvatarActorFromActorInfo());
	if (!IsValid(AvatarActor))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);	
	}
	AvatarActor->SetBlackBoardShouldExecute(false);

	DevSettings = GetDefault<URetroTestSettings>();
	
	// Play montage and wait for event
	URTAT_PLayMontageAndWaitForEvent* Task = URTAT_PLayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None,
		AutoAttackMontage, FGameplayTagContainer(), 1.0f, NAME_None, false, 1.0f);
	Task->OnBlendOut.AddDynamic(this, &UNPCAutoAttackAbility::OnCompleted);
	Task->OnCompleted.AddDynamic(this, &UNPCAutoAttackAbility::OnCompleted);
	Task->OnInterrupted.AddDynamic(this, &UNPCAutoAttackAbility::OnCancelled);
	Task->OnCancelled.AddDynamic(this, &UNPCAutoAttackAbility::OnCancelled);
	Task->EventReceived.AddDynamic(this, &UNPCAutoAttackAbility::EventReceived);
	Task->ReadyForActivation();
}

void UNPCAutoAttackAbility::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UNPCAutoAttackAbility::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UNPCAutoAttackAbility::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	// Montage told us to end the ability before the montage finished playing
	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.EndAbility")))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}
	
	if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority && EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.ExecuteSweep")))
	{
		if (!IsValid(AvatarActor))
		{
			EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
			return;
		}
		
		SweepForTargets();
	}
}

void UNPCAutoAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (IsValid(AvatarActor))
	{
		AvatarActor->SetBlackBoardShouldExecute(true);	
	}
}

void UNPCAutoAttackAbility::SweepForTargets() const
{
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
			if (const auto HitActor = Cast<ARetroTestPlayerCharacter>(Hit.GetActor()))
			{
				UAbilitySystemComponent* OwnerAbilitySystem = HitActor->GetAbilitySystemComponent();
				FGameplayEffectContextHandle Context = OwnerAbilitySystem->MakeEffectContext();
				FGameplayEffectSpecHandle SpecHandle = OwnerAbilitySystem->MakeOutgoingSpec(AbilityEffect, 1, Context);

				TArray<TWeakObjectPtr<AActor>> TargetActors;
				TargetActors.Add(HitActor);
				Context.AddActors(TargetActors);
				Context.AddHitResult(Hit);

				URetroTestAbilitySystemComponent* TargetAbilitySystem = HitActor->FindComponentByClass<URetroTestAbilitySystemComponent>();

				if (!IsValid(TargetAbilitySystem))
					return;
				
				OwnerAbilitySystem->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetAbilitySystem);
			}
		}
	}
}
