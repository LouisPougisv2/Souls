// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "MainCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Enemy.h"
#include "Engine/SkeletalMeshSocket.h"


AWeapon::AWeapon()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());

	CombatCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollisionBox->SetupAttachment(GetRootComponent());

	bWeaponParticles = false;
	Damage = 50.0f;

	WeaponState = EWeaponState::EWS_Pickup;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	//We need to bind this funcons to the OnComponentBeginOverlap
	//So OverlapBegin & OverlpadEnd function can be called each time sometime
	//Overlap with the Weapon Collision box
	CombatCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::WeaponOverlapBegin);
	CombatCollisionBox->OnComponentEndOverlap.AddDynamic(this, &AWeapon::WeaponOverlapEnd);

	//Collision is set to NoCollision so we wait for ActivateCollision() to have the behavours we're looking for
	CombatCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}

void AWeapon::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if ((WeaponState == EWeaponState::EWS_Pickup) && OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		
		if (MainCharacter)
		{
			MainCharacter->SetActiveOverlappingItem(this);
		}
	}
}

void AWeapon::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);

		if (MainCharacter)
		{
			MainCharacter->SetActiveOverlappingItem(nullptr);
		}
	}
}

void AWeapon::WeaponOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		if (Enemy)
		{
			if (Enemy->OnHitParticles)
			{
				const USkeletalMeshSocket* WeaponSocket = SkeletalMesh->GetSocketByName("WeaponSocket");
				if (WeaponSocket)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Enemy->OnHitParticles, WeaponSocket->GetSocketLocation(SkeletalMesh), FRotator(0.0f), false);
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("Enemy is taking damage"));
			Enemy->DecrementHealth(Damage);
		}
	}
}

void AWeapon::WeaponOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AEnemy* Enemy = Cast<AEnemy>(OtherActor);
		USphereComponent* EnemyBody = Cast<USphereComponent>(OverlappedComponent);
		//We need to verify that the SphereComponent we're overlapping is the combat spehere, not the aggro one!!!!
		if (Enemy && EnemyBody)
		{
			UE_LOG(LogTemp, Warning, TEXT("Weapon attack on overlap end"));
			Enemy->DecrementHealth(Damage);
		}
	}
}

void AWeapon::Equip(AMainCharacter* character)
{
	if (character)
	{
		//Set collision to ignore for the camera so the camera doesn't zoom in on the player if the sword 
		//gets between the character and the camera
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
		SkeletalMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

		SkeletalMesh->SetSimulatePhysics(false);

		//reference to the right hand socket from our skeletalmesh
		const USkeletalMeshSocket* RightHandSocket = character->GetMesh()->GetSocketByName("RightHandWeaponSocket");
		
		if (RightHandSocket)
		{
			RightHandSocket->AttachActor(this, character->GetMesh());
			bIsRotating = false;

			character->SetEquippedWeapon(this);
		}
		if (OnEquipSound)
		{
			//Playing sound when equiped
			UGameplayStatics::PlaySound2D(this, OnEquipSound);
		}
		if (bWeaponParticles)
		{
			IdleParticlesComponent->Deactivate();
		}
	}
}

void AWeapon::ActivateCollision()
{
	CombatCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

}

void AWeapon::DeactivateCollision()
{
	CombatCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}
