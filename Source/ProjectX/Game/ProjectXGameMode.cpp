// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectXGameMode.h"
#include "ProjectXPlayerController.h"
#include "ProjectX/Character/ProjectXCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjectXGameMode::AProjectXGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AProjectXPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	/* static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprint/Character/BP_Character"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	*/
}

void AProjectXGameMode::PlayerCharacterDead()
{
}
