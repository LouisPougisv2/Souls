// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemStorage.generated.h"

UCLASS()
class SOULS_API AItemStorage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemStorage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	//EditDeafultsOnly so we can set the map from the blueprint itself and allowing us to save the weapons data such
	//as the particles, sounds cue,... without having to worry which PS or SC belongs to which weapon
	UPROPERTY(EditDefaultsOnly, Category = "SavedData")
	TMap<FString, TSubclassOf<class AWeapon>> WeaponMap;

};
