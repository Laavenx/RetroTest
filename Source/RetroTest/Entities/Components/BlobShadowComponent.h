// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlobShadowComponent.generated.h"

/*
 * Use cylinder static mesh for this component, which has pivot at its center
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RETROTEST_API UBlobShadowComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UBlobShadowComponent();

private:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Shadow, meta = (AllowPrivateAccess = "true"))
	float BlobDrawDistance = 2500;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Shadow, meta = (AllowPrivateAccess = "true"))
	float BlobShrinkStartDistance = 500;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Shadow, meta = (AllowPrivateAccess = "true"))
	float BlobMaxRadius = 50;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Shadow, meta = (AllowPrivateAccess = "true"))
	float BlobMinRadius = 35;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Shadow, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialParameterCollection> BlobMaterialInstance;
	
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
