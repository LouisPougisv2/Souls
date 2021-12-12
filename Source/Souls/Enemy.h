// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyMovementStatus : uint8
{
	EMS_Idle			UMETA(DisplayName = "Idle"),
	EMS_MoveToTarget	UMETA(DisplayName = "MoveToTarget"),
	EMS_Attacking		UMETA(DisplayName = "Attacking"),
	EMS_Dead			UMETA(DisplayName = "Dead"),
	
	EMS_MAX				UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class SOULS_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	EEnemyMovementStatus EnemyMovementStatus;

	FORCEINLINE void SetEnemyMovementStatus(EEnemyMovementStatus newStatus) { EnemyMovementStatus = newStatus;  }
	FORCEINLINE EEnemyMovementStatus GetEnemyMovementStatus() { return EnemyMovementStatus; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI | General")
	class USphereComponent* AgroSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI | General")
	USphereComponent* CombatSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI | General")
	class AAIController* AIController;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Combat")
	bool bOverlappingCombatSphere;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = "Combat")
	AMainCharacter* CombatTarget;

	bool bHasValidTarget; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Sound")
	class USoundCue* SwingSound;

	/*.......................              Enemy Stats            ......................................*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Stats")
	float health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Stats")
	float maxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Stats")
	float damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Stats")
	class UParticleSystem* OnHitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Sound")
	class USoundCue* HitSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI | Combat")
	class UBoxComponent* CombatCollisionBox;

	//to make sure that we're actually paying the combat anim montage
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UAnimMontage* CombatMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking;


	FTimerHandle TimerAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackMinTime;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackMaxTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;


	FTimerHandle DeathTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float DeathDelay;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	virtual void AgroSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void AgroSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void CombatSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void CombatSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//OnOverlap functions for the weapon collision
	UFUNCTION()
	void ClawOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void ClawOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	void MoveToTarget(class AMainCharacter* Target);

	UFUNCTION(BlueprintCallable)
	void ActivateCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	void Attack();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void Die(AActor* Killer);

	//Function call from the blueprint from the animNotify
	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	const bool isAlive() { return GetEnemyMovementStatus() != EEnemyMovementStatus::EMS_Dead; }

	void Disappear();

};
