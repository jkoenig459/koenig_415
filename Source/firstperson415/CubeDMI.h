// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "CubeDMI.generated.h"

class UNiagaraSystem;


UCLASS()
class FIRSTPERSON415_API ACubeDMI : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACubeDMI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* boxComp;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* cubeMesh;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* baseMat;

	UPROPERTY()
	UMaterialInstanceDynamic* dmiMat;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* colorP;


	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);



};
