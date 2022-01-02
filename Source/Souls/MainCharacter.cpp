// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"
#include "MainPlayerController.h"
#include "SoulsSaveGame.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*Create Camera Boom (pulls toward the player if there's a collision*/
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.0f; //Camera follows at this distance
	CameraBoom->bUsePawnControlRotation = true; //Rotate arm based on Controller

	//Size for collision capsule
	GetCapsuleComponent()->SetCapsuleSize(47.83f, 109.43f);

	/*Create Follow Camera*/
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of tjhe boom and let the boom adjust to match
	//the controller orientation
	
	FollowCamera->bUsePawnControlRotation = false; 

	//Set our turn rate for input
	BaseTurnRate = 35.0f;
	BaseLookUpRate = 35.0f;

	//If we don't want the character to rotate along with the rotation
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	//If bOrientRotationToMovement = true, the character will automoatically turns toward that he's physically moving
	GetCharacterMovement()->bOrientRotationToMovement = true; //Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); //... at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 650.0f; //Jump's velocity
	GetCharacterMovement()->AirControl = 0.2f;

	maxHealth = 100.0f;
	health = 70.0f;
	maxStamina = 150.0f;
	stamina = 120.0f;
	coins = 0;

	//To refactor 
	runningSpeed = 450.0f; 
	sprintingSpeed = runningSpeed * 1.3f;

	bShiftKeyDown = false;
	bLMBDown = false;

	//Initialize Enum
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	StaminaDrainRate = 25.0f;
	StaminaMinToSprint = 50.0f;

	bIsAttacking = false;
	bHasCombatTarget = false;
	bIsMovingForward = false;
	bIsMovingRight = false;

	InterpolationSpeed = 15.0f;
	bIsInterpolatingToEnemy = false;

}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Next is put in begin play because we want to be sure to actually have our controller set before trying to set this variable value
	MainPlayerController = Cast<AMainPlayerController>(GetController());
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	float deltaStamina = StaminaDrainRate * DeltaTime;

	UseStamina(deltaStamina);

	if (bIsInterpolatingToEnemy && CombatTarget)
	{
		//Destination for our rotation
		FRotator LookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		//Next line gives us the correct rotation for the frame
		FRotator InterpolationRotation = FMath::RInterpTo(GetActorRotation(), LookAtYaw, DeltaTime, InterpolationSpeed);
		SetActorRotation(InterpolationRotation);

	}
	
	if (CombatTarget)
	{
		CombatTargetLocation = CombatTarget->GetActorLocation();
		if (MainPlayerController)
		{
			//update the enemy location in the MainPlayerController
			MainPlayerController->EnemyLocation = CombatTargetLocation;
		}
	}
}

FRotator AMainCharacter::GetLookAtRotationYaw(FVector TargetLocation)
{
	//FindLookAtRotation is coming from the UKismetMath library
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);
	FRotator LookAtRotationYaw(0.0f, LookAtRotation.Yaw, 0.0f);
	return LookAtRotationYaw;
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);	

	check(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AMainCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &ACharacter::StopJumping);
	
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Pressed, this, &AMainCharacter::StartSprinting);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Released, this, &AMainCharacter::StopSprinting);

	PlayerInputComponent->BindAction(TEXT("LMB"), EInputEvent::IE_Pressed, this, &AMainCharacter::LMBDown);
	PlayerInputComponent->BindAction(TEXT("LMB"), EInputEvent::IE_Released, this, &AMainCharacter::LMBUp);

	//Movements for the character
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AMainCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AMainCharacter::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis(TEXT("TurnRate"), this, &AMainCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, & AMainCharacter::LookUpAtRate);
}

void AMainCharacter::MoveForward(float Value)
{
	//set to false at everyfraame in that sense, the bool will only be equal to true if the key is pressed
	bIsMovingForward = false;
	if ((Controller != nullptr) && (Value != 0) && !bIsAttacking && (MovementStatus != EMovementStatus::EMS_Dead))
	{
		//the direction that the controller is facing
		const FRotator rotation = Controller->GetControlRotation(); 
		const FRotator YawRotation(0.0f, rotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, Value);
		bIsMovingForward = true;
	}
}
void AMainCharacter::MoveRight(float Value)
{
	bIsMovingRight = false;
	if ((Controller != nullptr) && (Value != 0) && !bIsAttacking && (MovementStatus != EMovementStatus::EMS_Dead))
	{
		//the direction that the controller is facing
		const FRotator rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, rotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, Value);
		bIsMovingRight = true;
	}
}

void AMainCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}


void AMainCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMainCharacter::LMBDown()
{
	bLMBDown = true;

	//If we're dead, we don't want to be able to pick up weapon nor attack
	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	if (ActiveOverlappingItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);
		}
	}
	else if (EquippedWeapon)
	{
		Attack();
	}
}

void AMainCharacter::LMBUp()
{
	bLMBDown = false;
}

void AMainCharacter::Attack()
{
	if (!bIsAttacking && (MovementStatus != EMovementStatus::EMS_Dead))
	{
		bIsAttacking = true;
		SetIsInterpolatingToEnemy(true);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && CombatMontage)
		{
//			int32 Section = FMath::RandRange(0, 1);
//			switch (Section)
//			{
//			case 0:
				AnimInstance->Montage_Play(CombatMontage, 2.2f);
				AnimInstance->Montage_JumpToSection(FName("Attack1"), CombatMontage);
//				break;
//			case 1:
//				AnimInstance->Montage_Play(CombatMontage, 1.8f);
//				AnimInstance->Montage_JumpToSection(FName("Attack2"), CombatMontage);
//				break;
//			default:
//				;
//			}
		}
	}
}

void AMainCharacter::PlayWeaponSwingSound()
{
		if (EquippedWeapon->SwingSound)
		{
			UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
		}
}

//if LMB is still down, he player keeps on attacking
void AMainCharacter::AttackEnd()
{
	bIsAttacking = false;
	SetIsInterpolatingToEnemy(false);
	if (bLMBDown)
	{
		Attack();
	}
}

void AMainCharacter::DecrementHealth(float damage)
{
//
}

float AMainCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if ((health - DamageAmount) <= 0)
	{
		health -= DamageAmount;
		Die();

		if (DamageCauser)
		{
			// the enemy will have this boolean that will be set to false whenever the character dies
			AEnemy* Enemy = Cast<AEnemy>(DamageCauser);
			if (Enemy)
			{
				Enemy->bHasValidTarget = false;
			}
		}
	}
	health -= DamageAmount;


	return DamageAmount;
}

void AMainCharacter::Die()
{
	if (MovementStatus == EMovementStatus::EMS_Dead) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CombatMontage)
	{
		AnimInstance->Montage_Play(CombatMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(FName("Death"), CombatMontage);
	}
	SetMovementStatus(EMovementStatus::EMS_Dead);
}

void AMainCharacter::Jump()
{
	if (MovementStatus != EMovementStatus::EMS_Dead)
	{
		ACharacter::Jump();
	}
}

void AMainCharacter::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}

void AMainCharacter::IncrementCoins(int32 coinValue)
{
	coins += coinValue;
}

void AMainCharacter::IncrementHealth(float healValue)
{
	if (health + healValue >= maxHealth)
	{
		health = maxHealth;
	}
	else
	{
		health += healValue;
	}
}

void AMainCharacter::UpdateCombatTarget()
{
	TArray<AActor*> OverlappingActors;
	//fill OverlappingActors with the enemy overlapping with the player
	GetOverlappingActors(OverlappingActors, EnemyFilter);

	if (OverlappingActors.Num() == 0)
	{
		//if there's no other enemy overlapping, we want to remove the enemy health bar widget from the viewport
		if (MainPlayerController)
		{
			MainPlayerController->HideEnemyHealthBar();
		}
		return;
	}

	//We're assuming that the first enemy in the array is the closest
	AEnemy* ClosestEnemy = Cast<AEnemy>(OverlappingActors[0]);

	if (ClosestEnemy)
	{
		//to reduce the number of calls to the GetActorLocation function
		FVector Location = GetActorLocation();
		float DistanceToClosestEnemy = (ClosestEnemy->GetActorLocation() - Location).Size();

		for (auto Actor : OverlappingActors)
		{
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			if (Enemy)
			{
				float DistanceToActor = (Enemy->GetActorLocation() - Location).Size();
				if (DistanceToActor < DistanceToClosestEnemy)
				{
					DistanceToClosestEnemy = DistanceToActor;
					ClosestEnemy = Enemy;
				}
			}
		}
		if (MainPlayerController)
		{
			MainPlayerController->DisplayEnemyHealthBar();
		}
		SetCombatTarget(ClosestEnemy);
		bHasCombatTarget = true;
	}
}

void AMainCharacter::SwitchLevel(FName NewLevelName)
{
	UWorld* World = GetWorld();
	if (World)
	{
		const FString MapName = World->GetMapName();

		//an FName cannot be initialized with an FString but can be with an String literal (obtained by dereferencing the FString
		FName CurrentMapName(*MapName);

		//We can now compare them to check if the current level is the same as the one the player is trying to transition into
		if (CurrentMapName != NewLevelName)
		{
			UGameplayStatics::OpenLevel(World, NewLevelName);
		}
	}

}

void AMainCharacter::SetMovementStatus(EMovementStatus status)
{
	MovementStatus = status;	
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = sprintingSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = runningSpeed;
	}
}


void AMainCharacter::StartSprinting()
{
	if (bIsMovingForward || bIsMovingRight)
	{
		bShiftKeyDown = true;
	}
}


void AMainCharacter::StopSprinting()
{
	bShiftKeyDown = false;
}

void AMainCharacter::UseStamina(float deltaStamina)
{
	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal:
		if (bShiftKeyDown)
		{
			if (stamina - deltaStamina <= StaminaMinToSprint)
			{
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
				stamina -= deltaStamina;
			}
			else
			{
				stamina -= deltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Sprinting);
		}
		else //Shift Key up, we need to check if we replenish the stamina (not above the max stamina value)
		{
			if (stamina + deltaStamina >= maxStamina)
			{
				stamina = maxStamina;
			}
			else
			{
				stamina += deltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_BelowMinimum:
		if (bShiftKeyDown)
		{
			if (stamina - deltaStamina <= 0.0f)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
				stamina = 0.0f;
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			else
			{
				stamina -= deltaStamina;
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
			}
		}
		else //Shift Key up
		{
			if (stamina + deltaStamina >= StaminaMinToSprint)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				stamina += deltaStamina;
			}
			else
			{
				stamina += deltaStamina;
			}
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	case EStaminaStatus::ESS_Exhausted:
		if (bShiftKeyDown)
		{
			stamina = 0.0f;
		}
		else //Shift Key up
		{
			SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
			stamina += deltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;

	case EStaminaStatus::ESS_ExhaustedRecovering:

		if (stamina + deltaStamina >= StaminaMinToSprint)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			stamina += deltaStamina;
		}
		else
		{
			stamina += deltaStamina;
		}
		SetMovementStatus(EMovementStatus::EMS_Normal);
		break;

	default:
		;
	}
}

void AMainCharacter::ShowPickupLocation()
{

	for (auto location : PickupLocations)
	{
		//Draw Debug sphere where items have been picked up (for debug sphere tests only)
		UKismetSystemLibrary::DrawDebugSphere(this, location, 25.f, 12, FLinearColor::Green, 20.0f, 0.5f);
	}
}

void AMainCharacter::SetEquippedWeapon(AWeapon* WeaponToSet)
{ 
//we need to destroy if there's a weapon already equipped before equipping a new one
	if (EquippedWeapon)
	{
		GetEquippedWeapon()->Destroy();
	}
	EquippedWeapon = WeaponToSet; 
}


void AMainCharacter::SaveGame()
{
	//we create a USoulsSaveGame pointer
	USoulsSaveGame* SaveGameInstance = Cast<USoulsSaveGame>(UGameplayStatics::CreateSaveGameObject(USoulsSaveGame::StaticClass()));

	SaveGameInstance->CharacterStats.Health = health;
	SaveGameInstance->CharacterStats.MaxHealth = maxHealth;
	SaveGameInstance->CharacterStats.Stamina = stamina;
	SaveGameInstance->CharacterStats.MaxStamina = maxStamina;
	SaveGameInstance->CharacterStats.Coins = coins;

	SaveGameInstance->CharacterStats.CharacterLocation = GetActorLocation();
	SaveGameInstance->CharacterStats.CharacterRotation = GetActorRotation();

	//Actually saving datas to the memory
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->PlayerSlot, SaveGameInstance->UserSlotIndex);
}

void AMainCharacter::LoadGame(bool SetPosition)
{
	USoulsSaveGame* LoadGameInstance = Cast<USoulsSaveGame>(UGameplayStatics::CreateSaveGameObject(USoulsSaveGame::StaticClass()));

	LoadGameInstance = Cast<USoulsSaveGame>(UGameplayStatics::LoadGameFromSlot(LoadGameInstance->PlayerSlot, LoadGameInstance->UserSlotIndex));

	health = LoadGameInstance->CharacterStats.Health;
	maxHealth = LoadGameInstance->CharacterStats.MaxHealth;
	stamina = LoadGameInstance->CharacterStats.Stamina;
	maxStamina = LoadGameInstance->CharacterStats.MaxStamina; 
	coins = LoadGameInstance->CharacterStats.Coins;

	if (SetPosition)
	{
		SetActorLocation(LoadGameInstance->CharacterStats.CharacterLocation);
		SetActorRotation(LoadGameInstance->CharacterStats.CharacterRotation);
	}
}