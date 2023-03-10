// Copyright Epic Games, Inc. All Rights Reserved.

#include "FlockingSystemGameMode.h"
#include "FlockingSystemPlayerController.h"
#include "FlockingSystemCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFlockingSystemGameMode::AFlockingSystemGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AFlockingSystemPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

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
