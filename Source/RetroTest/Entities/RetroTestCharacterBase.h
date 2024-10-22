// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RetroTestCharacterBase.generated.h"

class URetroTestAttributeSetBase;
class URetroTestAbilitySystemComponent;

UCLASS()
class RETROTEST_API ARetroTestCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ARetroTestCharacterBase(const FObjectInitializer& object_initializer);

public:
	virtual void BeginPlay() override;
	virtual void Die();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Attributes")
	float InitMaxHealth = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS - Attributes")
	float InitHealth = 0;

public:
	URetroTestAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComp; }
	URetroTestAttributeSetBase* GetAttributeSet() const { return AttributeSet; }
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URetroTestAbilitySystemComponent> AbilitySystemComp;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<URetroTestAttributeSetBase> AttributeSet;
	
};
