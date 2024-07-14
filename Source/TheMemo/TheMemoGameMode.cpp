// Copyright Epic Games, Inc. All Rights Reserved.

#include "TheMemoGameMode.h"
#include "TheMemoCharacter.h"
#include "TheMemoPlayerController.h"
#include "UObject/ConstructorHelpers.h"

ATheMemoGameMode::ATheMemoGameMode()
{
	// set default pawn class to our Blueprinted character
	DefaultPawnClass = ATheMemoCharacter::StaticClass();
	PlayerControllerClass = ATheMemoPlayerController::StaticClass();
}
