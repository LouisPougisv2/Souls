// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacterAnimInstance.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MainCharacter.h"

void UMainCharacterAnimInstance::NativeInitializeAnimation() 
{
	if (CharacterPawn == nullptr)
	{
		CharacterPawn = TryGetPawnOwner();
		if (CharacterPawn)
		{
			Main = Cast<AMainCharacter>(CharacterPawn);
		}
	}
}

void UMainCharacterAnimInstance::UpdateAnimationProperties()
{
	if (CharacterPawn == nullptr)
	{
		CharacterPawn = TryGetPawnOwner();
	}

	if (CharacterPawn)
	{
		FVector Speed = CharacterPawn->GetVelocity();
		FVector NewLaterSpeed = FVector(Speed.X, Speed.Y, 0.0f);
		MovementSpeed = NewLaterSpeed.Size();
		
		bIsInAir = CharacterPawn->GetMovementComponent()->IsFalling();

		if (Main == NULL)
		{
			Main = Cast<AMainCharacter>(CharacterPawn);
		}

	}	
}

