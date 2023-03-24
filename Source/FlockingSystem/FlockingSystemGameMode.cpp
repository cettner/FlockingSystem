// Copyright Epic Games, Inc. All Rights Reserved.

#include "FlockingSystemGameMode.h"
#include "FlockingSystemPlayerController.h"
#include "FlockingSystemCharacter.h"
#include "RTSCamera.h"

#include "UObject/ConstructorHelpers.h"




AFlockingSystemGameMode::AFlockingSystemGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AFlockingSystemPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	DefaultPawnClass = ARTSCamera::StaticClass();

	FlockManager = CreateDefaultSubobject<UFlockNavManager>(TEXT("NavManager"));
}

void AFlockingSystemGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();


}

UFlockNavManager* AFlockingSystemGameMode::GetFlockManager() const
{
	return FlockManager;
}
