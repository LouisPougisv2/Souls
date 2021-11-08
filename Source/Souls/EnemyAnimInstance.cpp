// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "Enemy.h"

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	if (EnemyPawn == nullptr)
	{
		EnemyPawn = TryGetPawnOwner();
		if (EnemyPawn)
		{
			Enemy = Cast<AEnemy>(EnemyPawn);
		}
	}
}

void UEnemyAnimInstance::UpdateAnimationProperties()
{
	if (EnemyPawn == nullptr)
	{
		EnemyPawn = TryGetPawnOwner();
		if (EnemyPawn)
		{
			Enemy = Cast<AEnemy>(EnemyPawn);
		}
	}
	if (EnemyPawn)
	{
		FVector Speed = EnemyPawn->GetVelocity();
		FVector LateralSpeed = FVector(Speed.X, Speed.Y, 0.0f);
		MovementSpeed = LateralSpeed.Size();
	}
}