// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	IDLE,
	ATTACK,
	DAMAGED,
	DEAD,
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	NONE,
	SWORD,
	GUN,
};
