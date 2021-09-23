// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatingPlatform.generated.h"

UCLASS()
class SOULS_API AFloatingPlatform : public AActor
{
	GENERATED_BODY()


public:	
	// Sets default values for this actor's properties
	AFloatingPlatform();

	/*Mesh for the Floating Platform*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Floating Plateform")
	class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	FVector StartPoint;

	//As EndPoint is a widget, its location will be in function of the Mesh ones
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget = "true"))
	FVector EndPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Plateform")
	float InterpolationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Plateform")
	float InterpolationTime;

	FTimerHandle InterpolationStopTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Floating Plateform")
	bool bIsInterpolating;

	float TotalDistanceToCover;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ToggleIsInterpolating();

	void SwapVectors(FVector& V1, FVector& V2);

};
