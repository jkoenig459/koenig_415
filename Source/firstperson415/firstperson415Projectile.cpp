// Copyright Epic Games, Inc. All Rights Reserved.

#include "firstperson415Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

Afirstperson415Projectile::Afirstperson415Projectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
	CollisionComp->OnComponentHit.AddDynamic(this, &Afirstperson415Projectile::OnHit);

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	ballMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ball Mesh"));

	// Set as root component
	RootComponent = CollisionComp;
	ballMesh->SetupAttachment(CollisionComp);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void Afirstperson415Projectile::BeginPlay()
{
	Super::BeginPlay();

	// Prevent immediate self-collisions generating hit spam
	if (AActor* OwningActor = GetOwner())
	{
		CollisionComp->IgnoreActorWhenMoving(OwningActor, true);
	}
	if (APawn* Inst = GetInstigator())
	{
		CollisionComp->IgnoreActorWhenMoving(Inst, true);
	}

	randColor = FLinearColor(
		UKismetMathLibrary::RandomFloatInRange(0.f, 1.f),
		UKismetMathLibrary::RandomFloatInRange(0.f, 1.f),
		UKismetMathLibrary::RandomFloatInRange(0.f, 1.f),
		1.f
	);

	// Material setup (guarded)
	if (projMat && ballMesh)
	{
		dmiMat = UMaterialInstanceDynamic::Create(projMat, this);
		if (dmiMat)
		{
			ballMesh->SetMaterial(0, dmiMat);
			dmiMat->SetVectorParameterValue(TEXT("ProjColor"), randColor);
		}
	}
}

void Afirstperson415Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	// One-shot impact guard (Hit events can fire multiple times in the same physics step)
	if (bImpacted) return;
	bImpacted = true;

	// Immediately stop movement + collisions so no "extra" hits happen after destroy is requested
	if (ProjectileMovement)
	{
		ProjectileMovement->bShouldBounce = false;
		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->Deactivate();
	}

	SetActorEnableCollision(false);

	if (CollisionComp)
	{
		CollisionComp->SetNotifyRigidBodyCollision(false);
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (ballMesh)
	{
		ballMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ballMesh->SetHiddenInGame(true);
	}

	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	}

	// Niagara impact
	if (colorP)
	{
		if (UNiagaraComponent* ParticleComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			colorP,
			Hit.Location,
			Hit.Normal.Rotation()
		))
		{
			ParticleComp->SetVariableLinearColor(TEXT("RandomColor"), randColor);
		}
	}

	// Decal impact
	if (baseMat)
	{
		const float FrameNum = UKismetMathLibrary::RandomFloatInRange(0.f, 3.f);

		if (UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(
			GetWorld(),
			baseMat,
			FVector(UKismetMathLibrary::RandomFloatInRange(20.0f, 40.0f)),
			Hit.Location,
			Hit.Normal.Rotation(),
			0.f
		))
		{
			if (UMaterialInstanceDynamic* MatInstance = Decal->CreateDynamicMaterialInstance())
			{
				MatInstance->SetVectorParameterValue(TEXT("Color"), randColor);
				MatInstance->SetScalarParameterValue(TEXT("Frame"), FrameNum);
			}
		}
	}

	Destroy();
}
