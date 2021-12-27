// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Critter.h"
#include "Enemy.h"
#include "AIController.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
}
  
// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	//filling the spawn array with the actors
	if (ActorToSpawn_1 && ActorToSpawn_2 && ActorToSpawn_3)
	{
		SpawnActors.Add(ActorToSpawn_1);
		SpawnActors.Add(ActorToSpawn_2);
		SpawnActors.Add(ActorToSpawn_3);
	}
	
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//the extent is a vector that gives how big is the volume
}

FVector ASpawnVolume::GetSpawnPoint()
{
	FVector Extent = SpawningBox->GetScaledBoxExtent();
	FVector OriginOfTheBox = SpawningBox->GetComponentLocation();

	return UKismetMathLibrary::RandomPointInBoundingBox(OriginOfTheBox, Extent);
}

TSubclassOf<AActor> ASpawnVolume::GetSpawnActor()
{
	if (SpawnActors.Num() > 0)
	{
		int32 Selection = FMath::FRandRange(0, SpawnActors.Num() - 1);
		return SpawnActors[Selection];
	}	
	else 
	{
		return nullptr;
	}
}

//the +Implementation come from the BlueprintNativeEvents
void ASpawnVolume::SpawnOurActor_Implementation(UClass* ToSpawn, const FVector& Location)
{
	if (ToSpawn)
	{
		UWorld* World = GetWorld();
		FActorSpawnParameters SpawnParams;

		if(World)
		{
			AActor* ActorSpawned = World->SpawnActor<AActor>(ToSpawn, Location, FRotator(0.0f), SpawnParams);

			AEnemy* EnemySpawned = Cast<AEnemy>(ActorSpawned);
			if (EnemySpawned)
			{
				//Spawn an AIController for our EnemySpawned and actually set it
				EnemySpawned->SpawnDefaultController();

				AAIController* AIController = Cast<AAIController>(EnemySpawned->GetController());
				if (AIController)
				{ 
					//now that we have access to the AIController, the Controller will be able to move
					EnemySpawned->AIController = AIController;
				}
			}
		}
	}
}
