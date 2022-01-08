// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SOULS_API AMainPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	/*Reference to the UMG asset in the editor*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> HUDOverlayAsset;
	
	/*Varaible to hold the widget after creating it*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	UUserWidget* HUDOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " Widget")
	TSubclassOf<UUserWidget> WEnemyHealthBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widget")
	UUserWidget* EnemyHealthBar;

	// Pause Menu

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " Widget")
	TSubclassOf<UUserWidget> WPauseMenu;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Widget")
	UUserWidget* PauseMenu;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	FVector EnemyLocation;

	bool bEnemyHealthBarVisible;

	bool bPauseMenuVisible;

	void DisplayEnemyHealthBar();

	void HideEnemyHealthBar();

	void DisplayPauseMenu();
	void HidePauseMenu();
	void TogglePauseMenu();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
};
