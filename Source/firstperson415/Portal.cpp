// Fill out your copyright notice in the Description page of Project Settings.

#include "Portal.h"
#include "firstperson415Character.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

APortal::APortal()
{
	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Comp"));
	sceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Capture"));

	RootComponent = boxComp;
	mesh->SetupAttachment(boxComp);
	sceneCapture->SetupAttachment(mesh);

	mesh->SetCollisionResponseToAllChannels(ECR_Ignore);
}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();

	if (boxComp)
	{
		boxComp->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnOverlapBegin);
	}

	if (mesh)
	{
		mesh->SetHiddenInSceneCapture(true);

		if (mat)
		{
			mesh->SetMaterial(0, mat);
		}
	}
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdatePortals();
}

void APortal::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	Afirstperson415Character* playerChar = Cast<Afirstperson415Character>(OtherActor);

	if (playerChar && OtherPortal)
	{
		if (!playerChar->isTeleporting)
		{
			playerChar->isTeleporting = true;

			const FVector Loc = OtherPortal->GetActorLocation();
			playerChar->SetActorLocation(Loc);

			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate;

			// BindUFunction expects an FName
			TimerDelegate.BindUFunction(this, FName("SetBool"), playerChar);

			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 1.0f, false);
		}
	}
}

void APortal::SetBool(Afirstperson415Character* playerChar)
{
	if (playerChar)
	{
		playerChar->isTeleporting = false;
	}
}

void APortal::UpdatePortals()
{
	// Prevent crashes if not wired in editor / blueprint
	if (!OtherPortal || !sceneCapture)
	{
		return;
	}

	const FVector Location = GetActorLocation() - OtherPortal->GetActorLocation();

	APlayerCameraManager* Cam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (!Cam)
	{
		return;
	}

	const FVector camLocation = Cam->GetTransformComponent()->GetComponentLocation();
	const FRotator camRotation = Cam->GetTransformComponent()->GetComponentRotation();

	const FVector CombinedLocation = camLocation + Location;

	sceneCapture->SetWorldLocationAndRotation(CombinedLocation, camRotation);
}
