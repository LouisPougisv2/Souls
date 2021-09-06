// Fill out your copyright notice in the Description page of Project Settings.


#include "Critter.h"
#include "Camera/CameraComponent.h"

// Sets default values
ACritter::ACritter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create CritterMeshComponent and attached it to the root
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	CritterMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CritterMeshComponent"));
	CritterMeshComponent->SetupAttachment(GetRootComponent()); 

	//CameraComponent
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(GetRootComponent());
	CameraComponent->SetRelativeLocation(FVector(-300.f, 0.f, 0.f));
	CameraComponent->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));

	//To possess the default pawn (refers to the 1st player in Multiplayer games or the only one in an PVE game)
	//AutoPossessPlayer = EAutoReceiveInput::Player0;

	CurrentVelocity = FVector(0.0f);
	MaxSpeed = 200.0f;
}

// Called when the game starts or when spawned
void ACritter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACritter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
	SetActorLocation(NewLocation);

}

// Called to bind functionality to input
void ACritter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACritter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACritter::MoveRight);

}

void ACritter::MoveForward(float value)
{
	CurrentVelocity.X = FMath::Clamp(value, -1.0f, 1.0f) * MaxSpeed;
};

void ACritter::MoveRight(float value)
{ 
	CurrentVelocity.Y = FMath::Clamp(value, -1.0f, 1.0f) * MaxSpeed;
};