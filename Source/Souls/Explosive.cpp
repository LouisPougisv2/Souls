// Fill out your copyright notice in the Description page of Project Settings.


#include "Explosive.h"
#include "MainCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/World.h"



AExplosive::AExplosive()
{
	damage = 50.0f;
}

void AExplosive::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);


	if (OtherActor)
	{
		AMainCharacter* mainCharacter = Cast<AMainCharacter>(OtherActor);
		if (mainCharacter)
		{
			//Spawn the particle
			if (OverlapParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OverlapParticles, GetActorLocation(), FRotator(0.0f), true);
			}
			//Then destroy the Actor once we hit it 
			if (OverlapSound)
			{
				UGameplayStatics::PlaySound2D(this, OverlapSound);
			}

			//GetFirstPlayerController works perfectly only because we're on a PvP game, otherwise it would become a pbm
			UGameplayStatics::ApplyDamage(mainCharacter, damage, GetWorld()->GetFirstPlayerController(), this, DamageTypeClass);
			mainCharacter->PickupLocations.Add(GetActorLocation());

			Destroy();
		}
	}
}

void AExplosive::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

}