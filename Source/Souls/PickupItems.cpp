// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupItems.h"
#include "MainCharacter.h"

APickupItems::APickupItems()
{
	coinCount = 0;
	coinValue = 1;
}

void APickupItems::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	UE_LOG(LogTemp, Warning, TEXT("On Overlap Begin (PickupItems)"));

	if (OtherActor)
	{
		AMainCharacter* mainCharacter = Cast<AMainCharacter>(OtherActor);
		if (mainCharacter)
		{
			mainCharacter->IncrementCoins(coinValue);
			mainCharacter->PickupLocations.Add(GetActorLocation());
		}
	}

}

void APickupItems::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	UE_LOG(LogTemp, Warning, TEXT("On Overlap End (PickupItems)"));

}
