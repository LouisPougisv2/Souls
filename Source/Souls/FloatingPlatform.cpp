// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatingPlatform.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
// Sets default values
AFloatingPlatform::AFloatingPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	StartPoint = FVector(0.0f);
	EndPoint = FVector(0.0f);

	bInterpolating = false; 


}

// Called when the game starts or when spawned
void AFloatingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	StartPoint = GetActorLocation();

	//As the location is not a world location (but based on the one from the platform), we need to set the 
	//EndPoint as followed
	EndPoint += StartPoint;

	InterpolationSpeed = 1.0f;
	InterpolationTime = 1.0f;

	GetWorldTimerManager().SetTimer(InterpolationStopTimer, this, &AFloatingPlatform::ToggleInterpolating, InterpolationTime);

	Distance = (EndPoint - StartPoint).Size();
}

// Called every frame
void AFloatingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bInterpolating)
	{
		//VInterpTo stands for Vector Interpolation in a three dimensional space
		FVector CurrentLocation = GetActorLocation();
		FVector Interpolation = FMath::VInterpTo(CurrentLocation, EndPoint, DeltaTime, InterpolationSpeed);

		SetActorLocation(Interpolation);

		float DistanceTraveled = (GetActorLocation() - StartPoint).Size();
		if ((Distance - DistanceTraveled) <= 1.0f)
		{
			ToggleInterpolating();
			GetWorldTimerManager().SetTimer(InterpolationStopTimer, this, &AFloatingPlatform::ToggleInterpolating, InterpolationTime);
			SwapVectors(StartPoint, EndPoint);
		}
	}
	
}


void AFloatingPlatform::ToggleInterpolating() 
{
	bInterpolating = !bInterpolating;
}

void AFloatingPlatform::SwapVectors(FVector& V1, FVector& V2)
{
	FVector temp = V1;
	V1 = V2;
	V2 = temp;
}
