// Copyright Epic Games, Inc. All Rights Reserved.

#include "firstperson415PickUpComponent.h"

Ufirstperson415PickUpComponent::Ufirstperson415PickUpComponent()
{
	// Setup the Sphere Collision
	SphereRadius = 32.f;
}

void Ufirstperson415PickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register our Overlap Event
	OnComponentBeginOverlap.AddDynamic(this, &Ufirstperson415PickUpComponent::OnSphereBeginOverlap);
}

void Ufirstperson415PickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Checking if it is a First Person Character overlapping
	Afirstperson415Character* Character = Cast<Afirstperson415Character>(OtherActor);
	if(Character != nullptr)
	{
		// Notify that the actor is being picked up
		OnPickUp.Broadcast(Character);

		// Unregister from the Overlap Event so it is no longer triggered
		OnComponentBeginOverlap.RemoveAll(this);
	}
}
