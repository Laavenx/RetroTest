// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpBase.h"
#include "Components/SphereComponent.h"
#include "..\..\Player\RetroTestPlayerCharacter.h"
#include "RetroTest/GAS/Attributes/RetroTestPlayerAttributeSet.h"
#include "Kismet/GameplayStatics.h"


APickUpBase::APickUpBase()
{
	PrimaryActorTick.bCanEverTick = true;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupPickupMesh"));
	SetRootComponent(PickupMesh);
	PickupMesh->SetComponentTickEnabled(false);
	PickupMesh->SetGenerateOverlapEvents(false);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComponent);
	SphereCollision->SetGenerateOverlapEvents(true);
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &APickUpBase::OnSphereBeginOverlap);
}

void APickUpBase::BeginPlay()
{
	Super::BeginPlay();
}

void APickUpBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickUpBase::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ARetroTestPlayerCharacter* Player = Cast<ARetroTestPlayerCharacter>(OtherActor))
	{
		UGameplayStatics::PlaySoundAtLocation(this, Sound, GetActorLocation());
		UE_LOG(LogTemp, Log, TEXT("APickUpBase::OnSphereBeginOverlap Player interacted with pick up object"));
		if (auto AttribSet = Cast<URetroTestPlayerAttributeSet>(Player->GetAttributeSet()))
		{
			AttribSet->SetCoins(1);
		}
		Destroy();
	}
}

