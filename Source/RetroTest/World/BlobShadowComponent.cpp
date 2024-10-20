// Fill out your copyright notice in the Description page of Project Settings.


#include "BlobShadowComponent.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Kismet/KismetMathLibrary.h"

UBlobShadowComponent::UBlobShadowComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBlobShadowComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBlobShadowComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                         FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	FHitResult TraceOutHit;
	
	FCollisionQueryParams CollisionParameters = FCollisionQueryParams::DefaultQueryParam;
	CollisionParameters.AddIgnoredActor(GetOwner());
	
	FCollisionObjectQueryParams ObjectsToHit;
	ObjectsToHit.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectsToHit.AddObjectTypesToQuery(ECC_WorldDynamic);
	
	const auto OwnerLocation = GetOwner()->GetActorLocation();
	
	bool bIsBlockingHit = GetWorld()->LineTraceSingleByObjectType(TraceOutHit, OwnerLocation,
		OwnerLocation - FVector(0,0,BlobDrawDistance), ObjectsToHit, CollisionParameters);
	
	// controls blob shadow
	if (bIsBlockingHit)
	{
		this->SetWorldLocation(TraceOutHit.Location);
		this->SetVisibility(true);
		UMaterialParameterCollectionInstance* Instance = GetWorld()->GetParameterCollectionInstance(BlobMaterialInstance);
		const double Distance = FVector::Dist(OwnerLocation, TraceOutHit.Location);
		const float Alpha = Distance / BlobShrinkStartDistance;
		float ActorDistanceFromGround = UKismetMathLibrary::Lerp(BlobMaxRadius, BlobMinRadius,Alpha);
		Instance->SetScalarParameterValue(FName("ModifyRadius"), ActorDistanceFromGround);
	}
	else
	{
		this->SetVisibility(false);
	}
}

