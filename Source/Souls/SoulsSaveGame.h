// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SoulsSaveGame.generated.h"

//Structs in UE starts with an F
// next macro is to be sure that our struc will participate in the reflection system
USTRUCT(BlueprintType)
struct FCharacterStats
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "SavedGameDatas")
	float Health;

	UPROPERTY(VisibleAnywhere, Category = "SavedGameDatas")
	float MaxHealth;

	UPROPERTY(VisibleAnywhere, Category = "SavedGameDatas")
	float Stamina;

	UPROPERTY(VisibleAnywhere, Category = "SavedGameDatas")
	float MaxStamina;

	UPROPERTY(VisibleAnywhere, Category = "SavedGameDatas")
	int32 Coins;

	UPROPERTY(VisibleAnywhere, Category = "SavedGameDatas")
	FVector CharacterLocation;

	UPROPERTY(VisibleAnywhere, Category = "SavedGameDatas")
	FRotator CharacterRotation;

	UPROPERTY(EditDefaultsOnly, Category = "SavedData")
	FString WeaponName;
};

/**
 * 
 */
UCLASS()
class SOULS_API USoulsSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	USoulsSaveGame();	 

	// HAS TO BE UPROPERTY(VisibleAnywhere, Category = Basic)
	// has to be recognized by the reflection system in other to save these variables
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString PlayerSlot;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserSlotIndex;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	FCharacterStats CharacterStats;
	
};
