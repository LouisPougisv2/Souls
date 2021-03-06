 // Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "MainCharacter.h"
#include "MainPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"



// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgroSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgroSphere->SetupAttachment(GetRootComponent());
	AgroSphere->InitSphereRadius(600.0f);
	AgroSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Ignore);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(100.0f);

	CombatCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollisionBox->SetupAttachment(GetMesh(), FName("FrontLeftSocket"));
	//CombatCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("FrontLeftClawSocket"));
	
	bOverlappingCombatSphere = false;
	health = 140.0f;
	maxHealth = 150.0f;
	damage = 20.0f;
	
	bIsAttacking = false;
	bHasValidTarget = false;

	AttackMinTime = 0.5f;
	AttackMaxTime = 2.5f;

	EnemyMovementStatus = EEnemyMovementStatus::EMS_Idle;

	DeathDelay = 2.0f;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	//Cast Controller to AIController because GetController() return a AController but AIController is derived from AController (so more specific)
	AIController = Cast<AAIController>(GetController());

	//We now need to bind overlap events to thr overlap components
	AgroSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapBegin);
	AgroSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::AgroSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);

	//bind the combat collision box to the events
	CombatCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::ClawOnOverlapBegin);
	CombatCollisionBox->OnComponentEndOverlap.AddDynamic(this, &AEnemy::ClawOnOverlapEnd);

	//Collision Parameters
	CombatCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	//Our collision with camera will be ignored now (For capsule and mesh)
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && isAlive())
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			MoveToTarget(MainCharacter);
		}
	}
}

void AEnemy::AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			bHasValidTarget = false;
			if (MainCharacter->CombatTarget == this)
			{
				MainCharacter->SetCombatTarget(nullptr);
			}
			MainCharacter->bHasCombatTarget = false;
			
			MainCharacter->UpdateCombatTarget();

			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
			if (AIController)
			{
				AIController->StopMovement();
			}
		}
	}
}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && isAlive())
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			bHasValidTarget = true;
			MainCharacter->SetCombatTarget(this);
			MainCharacter->bHasCombatTarget = true;
			MainCharacter->UpdateCombatTarget();
			if (MainCharacter->MainPlayerController)
			{
				MainCharacter->MainPlayerController->DisplayEnemyHealthBar();
			}
			CombatTarget = MainCharacter;
			bOverlappingCombatSphere = true;
			Attack();
		}
	}
}

void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);

		if (MainCharacter)
		{
			bOverlappingCombatSphere = false;		
			if(EnemyMovementStatus != EEnemyMovementStatus::EMS_Attacking)
			{ 
				MoveToTarget(MainCharacter);
				CombatTarget = nullptr;
			}
			if (MainCharacter->CombatTarget == this)
			{
				MainCharacter->SetCombatTarget(nullptr);
				MainCharacter->bHasCombatTarget = false;
				MainCharacter->UpdateCombatTarget();
			}

			//if the timer (between 2 attacks) is running, the next line will reset/clear it
			GetWorldTimerManager().ClearTimer(TimerAttack);
		}	
	}
}

void AEnemy::ClawOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor);
		if (MainCharacter)
		{
			if (MainCharacter->OnHitParticles)
			{
				const USkeletalMeshSocket* ClawSocket = GetMesh()->GetSocketByName("TipLeftSocket");
				if (ClawSocket)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MainCharacter->OnHitParticles, ClawSocket->GetSocketLocation(GetMesh()), FRotator(0.0f), false);
				}
			}
			if (MainCharacter->HitSound)
			{
				UGameplayStatics::PlaySound2D(this, MainCharacter->HitSound);
			}
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(MainCharacter, damage, AIController, this, DamageTypeClass);
			}
		}
	}
}

void AEnemy::ClawOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AEnemy::MoveToTarget(AMainCharacter* Target)
{
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
	if (AIController)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(20.0f);

		FNavPathSharedPtr OutPath;

		AIController->MoveTo(MoveRequest, &OutPath);

		/*
		//Will return a TArray of NavPathPoint
		auto PathPoints = OutPath->GetPathPoints();
		//draw debug sphere
		for (auto points : PathPoints)
		{
			FVector Location = points.Location;
			UKismetSystemLibrary::DrawDebugSphere(this, Location, 25.f, 12, FLinearColor::White, 20.0f, 0.5f);
		}
		*/

	}
}

void AEnemy::ActivateCollision()
{
	CombatCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	//play the swing sound
	if (SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, SwingSound);
	}
}

void AEnemy::DeactivateCollision()
{
	CombatCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::Attack()
{
	if (isAlive() && bHasValidTarget )
	{
		if (AIController)
		{
			AIController->StopMovement();
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
		}
		if (!bIsAttacking)
		{
			bIsAttacking = true;
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

			//play the animMontage
			if (AnimInstance)
			{
				AnimInstance->Montage_Play(CombatMontage, 1.0f);
				AnimInstance->Montage_JumpToSection(FName("Attack"), CombatMontage);
			}
		}
	}
	
}
void AEnemy::AttackEnd()
{
	bIsAttacking = false;

	if (bOverlappingCombatSphere)
	{
		float AttackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
		//Set the world timer manager with the attacktime befor executing the attack function
		GetWorldTimerManager().SetTimer(TimerAttack, this, &AEnemy::Attack, AttackTime);
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if ((health - DamageAmount) <= 0)
	{
		health -= DamageAmount;
		Die(DamageCauser);
	}
	health -= DamageAmount;

	return DamageAmount;
}

void AEnemy::Die(AActor* Killer)
{
	UAnimInstance* EnemyAnimInstance = GetMesh()->GetAnimInstance();
	if (EnemyAnimInstance)
	{
		EnemyAnimInstance->Montage_Play(CombatMontage, 1.0f);
		EnemyAnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);
	}
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);

	CombatCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AgroSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bIsAttacking = false;

	AMainCharacter* MainCharacter = Cast<AMainCharacter>(Killer);
	if (MainCharacter)
	{
		MainCharacter->UpdateCombatTarget();
	}
}

void AEnemy::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
	
	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);
}

void AEnemy::Disappear()
{
	Destroy();
}
