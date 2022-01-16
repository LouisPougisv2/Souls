// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulsSaveGame.h"

USoulsSaveGame::USoulsSaveGame()
{
	PlayerSlot = TEXT("Default");
	UserSlotIndex = 0;

	CharacterStats.WeaponName = TEXT("");
	CharacterStats.MapName = TEXT("");
}