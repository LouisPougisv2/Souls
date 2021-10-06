// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "PickupItems.generated.h"

/**
 * 
 */
UCLASS()
class SOULS_API APickupItems : public AItem
{
	GENERATED_BODY()
	
public:

	APickupItems();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coins")
	int32 coinCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coins")
	int32 coinValue;

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	void IncreaseCoinCount();
};
