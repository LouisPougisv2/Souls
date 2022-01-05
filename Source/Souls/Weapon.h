// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Weapon.generated.h"

UENUM (BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Pickup UMETA(DisplayName = "Pickup"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),

	EWS_MAX UMETA(DisplayName = "DefaultMax")
};
UCLASS()
class SOULS_API AWeapon : public AItem
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

public:

	AWeapon();

	UPROPERTY(EditDefaultsOnly, Category = "SavedData")
	FString WeaponName;

	EWeaponState WeaponState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
	bool bWeaponParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SkeletalMesh")
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	class UBoxComponent* CombatCollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
	class USoundCue* OnEquipSound;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Sound")
	USoundCue* SwingSound;

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	//OnOverlap functions for the weapon collision
	UFUNCTION()
	void WeaponOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void WeaponOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void Equip(class AMainCharacter* character);

	UFUNCTION(BlueprintCallable)
	void ActivateCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	//weapon instigator
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	AController* WeaponInstigator;

	FORCEINLINE void SetInstigator(AController* newInstigator) { WeaponInstigator = newInstigator; }

	FORCEINLINE void SetWeaponState(EWeaponState newWeaponState) {		WeaponState = newWeaponState;	}

	FORCEINLINE EWeaponState GetWeaponState(){		return WeaponState;	}
};
