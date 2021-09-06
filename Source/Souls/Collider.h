// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Collider.generated.h"

//This new pawn will allow us to turn and to look around in the world
UCLASS()
class SOULS_API ACollider : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACollider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	UStaticMeshComponent* MeshComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class USphereComponent* SphereComponent;

	//Camera
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent* Camera;

	//Spring arm to help control de camera's motion
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class USpringArmComponent* SpringArm; 

	//Collider Component
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	class UColliderMovementComponent* OurMovementComponent;

	virtual UPawnMovementComponent* GetMovementComponent() const override;

	// ---------------------------- Getters & Setter ------------------------------
	
	//mesh component
	FORCEINLINE UStaticMeshComponent* getColliderMeshComponent() { return MeshComponent;}
	FORCEINLINE void setColliderMeshComponent(UStaticMeshComponent* Mesh) { MeshComponent = Mesh; }

	//sphere
	FORCEINLINE USphereComponent* getSphereComponent() { return SphereComponent; }
	FORCEINLINE void setSphereComponent(USphereComponent* Sphere) { SphereComponent = Sphere; }

	//camera
	FORCEINLINE UCameraComponent* getCameraComponent() { return Camera; }
	FORCEINLINE void setCameraComponent(UCameraComponent* newCamera) { Camera = newCamera; }

	//spring Arm
	FORCEINLINE USpringArmComponent* getSpringArmComponent() { return SpringArm; }
	FORCEINLINE void setSpringArmComponent(USpringArmComponent* springArm) { SpringArm = springArm; }

	// ---------------------------- END of Getters & Setter ------------------------------

private:

	void MoveForward(float input);
	void MoveRight(float input);

	void PitchCamera(float axisValue);
	void YawCamera(float axisValue);

	//store some informations about the X & Y value
	FVector2D CameraInput;

};
