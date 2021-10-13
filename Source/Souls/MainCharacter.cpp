// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	//Initialize Enum
	MovementStatus = EMovementStatus::EMS_Normal;
	StaminaStatus = EStaminaStatus::ESS_Normal;

	StaminaDrainRate = 25.0f;
	StaminaMinToSprint = 50.0f;

}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float deltaStamina = StaminaDrainRate * DeltaTime;

	switch (StaminaStatus)
	{
	case EStaminaStatus::ESS_Normal :
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
	case EStaminaStatus::ESS_BelowMinimum :
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

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);	

	check(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &ACharacter::StopJumping);
	
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Pressed, this, &AMainCharacter::StartSprinting);
	PlayerInputComponent->BindAction(TEXT("Sprint"), EInputEvent::IE_Released, this, &AMainCharacter::StopSprinting);

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
	if ((Controller != nullptr) && (Value != 0))
	{
		//the direction that the controller is facing
		const FRotator rotation = Controller->GetControlRotation(); 
		const FRotator YawRotation(0.0f, rotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, Value);
	}
}
void AMainCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0))
	{
		//the direction that the controller is facing
		const FRotator rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, rotation.Yaw, 0.0f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, Value);
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


void AMainCharacter::DecrementHealth(float damage)
{
	if ( (health - damage) <= 0)
	{
		health -= damage;
		Die();
	}
	health -= damage;
}

void AMainCharacter::Die()
{
	//TO FILL
}

void AMainCharacter::IncrementCoins(int32 coinValue)
{
	coins += coinValue;
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
	bShiftKeyDown = true;
}


void AMainCharacter::StopSprinting()
{
	bShiftKeyDown = false;
}