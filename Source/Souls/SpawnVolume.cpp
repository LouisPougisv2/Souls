// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Critter.h"
#include "AIController.h"
#include "Enemy.h"
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
		int32 Selection = FMath::RandRange(0, SpawnActors.Num() - 1);
		return SpawnActors[Selection];
	}		
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("GetSpawnActor Check failed"));
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
			AActor* Actor = World->SpawnActor<AActor>(ToSpawn, Location, FRotator(0.0f), SpawnParams);

			AEnemy* EnemySpawned = Cast<AEnemy>(Actor);
			
			if (EnemySpawned)
			{
				//following lines allow the EnemySpawned to have a "brain", nont only a mesh
				EnemySpawned->SpawnDefaultController();

				AAIController* AICont = Cast<AAIController>(EnemySpawned->GetController());

				if (AICont)
				{
					EnemySpawned->AIController = AICont;
				}
			}
		}
	}
}
