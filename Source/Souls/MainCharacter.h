// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacter.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Dead UMETA(DisplayName = "Dead"),

	//not meant to be used, it's just kind of a default for the last one
	EMS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),

	ESS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class SOULS_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	//We're giving the Main character to choose from a drop down a weapon storage blueprint in this SubclassOf
	//the main character has now access to the weapon storage and access the correct weapon we saved based on the name of it
	UPROPERTY(EditDefaultsOnly, Category = " SavedData")
	TSubclassOf<class AItemStorage> WeaponStorage;

	//TArray used to try out the way they work
	TArray<FVector> PickupLocations;

	UFUNCTION(BlueprintCallable)
	void ShowPickupLocation();
	
	/*.......................              Movement Status            ......................................*/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EMovementStatus MovementStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus StaminaStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movements")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movements")
	float StaminaMinToSprint;

	//Method and member for avoiding to aim before attacking
	float InterpolationSpeed;

	bool bIsInterpolatingToEnemy;

	void SetIsInterpolatingToEnemy(bool value) { bIsInterpolatingToEnemy = value; }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	class AEnemy* CombatTarget;

	FORCEINLINE void SetCombatTarget(AEnemy* NewTarget) { CombatTarget = NewTarget; }

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bHasCombatTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	FVector CombatTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AMainPlayerController* MainPlayerController;


	FRotator GetLookAtRotationYaw(FVector TargetLocation);

	//------------------------------------------------------
	FORCEINLINE void SetStaminaStatus(EStaminaStatus status) { StaminaStatus = status; }

	void UseStamina(float deltaStamina);

	//Set Movement Status and running speed
	void SetMovementStatus(EMovementStatus status);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float runningSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Running")
	float sprintingSpeed;

	//Key to sprint
	bool bShiftKeyDown;

	//When the shift key is pressed down to enable sprinting
	void StartSprinting();

	//Released to stop sprinting
	void StopSprinting();

	/* Camera Boom positioning the camera behing the player*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/* Follow Camera*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	//Base turn rates to scale turning functions for the camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera);
	float BaseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera);
	float BaseLookUpRate;

	/*.......................              Player Stats            ......................................*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,  Category = "Player Stats")
	float maxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
	float maxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	float stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
	int32 coins;

	//EditDefaultsOnly because I don't want it to set it on each individual instance of the character
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Items")
	class AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	class AItem* ActiveOverlappingItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bIsAttacking;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* CombatMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Stats")
	class UParticleSystem* OnHitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat | Sound")
	class USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<AEnemy> EnemyFilter;

	void Attack();

	UFUNCTION(BlueprintCallable)
	void PlayWeaponSwingSound();

	UFUNCTION(BLueprintCallable)
	void AttackEnd();

	void DecrementHealth(float damage);

	virtual float TakeDamage(float DamageAmount,	struct FDamageEvent const& DamageEvent,	class AController* EventInstigator, AActor* DamageCauser) override;
	
	void Die();

	virtual void Jump() override;

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	UFUNCTION(BlueprintCallable)
	void IncrementCoins(int32 coinValue);

	UFUNCTION(BlueprintCallable)
	void IncrementHealth(float healValue);

	void UpdateCombatTarget();

	void SwitchLevel(FName NewLevelName);

	UFUNCTION(BlueprintCallable)
	void SaveGame();

	UFUNCTION(BlueprintCallable)
	void LoadGame(bool SetPosition);

	UFUNCTION(BlueprintCallable)
	void LoadGameNoSwitch();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	//Call forward/backward input
	void MoveForward(float Value);
	//Call for side to side inpout
	void MoveRight(float Value);

	bool bIsMovingForward;

	bool bIsMovingRight;

	/*Called via input to turn at a giving rate
	@param Rate this is normalized rate, i.e 1.0f means 100% of desired turned rate
	*/
	
	void TurnAtRate(float Rate);

	/*Called via input to look up/down at a giving rate
@param Rate this is normalized rate, i.e 1.0f means 100% of desired look up/down rate
*/
	void LookUpAtRate(float Rate);

	bool bLMBDown;
	void LMBDown();
	void LMBUp();

	bool bESCDown;
	void ESCDown();
	void ESCUp();


	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; };
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; };

	void SetEquippedWeapon(AWeapon* WeaponToSet);
	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item;	  };
	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; };

};
