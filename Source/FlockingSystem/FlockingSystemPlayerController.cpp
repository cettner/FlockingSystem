// Copyright Epic Games, Inc. All Rights Reserved.

#include "FlockingSystemPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "FlockingSystemCharacter.h"
#include "FlockAIController.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AFlockingSystemPlayerController::AFlockingSystemPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
}

void AFlockingSystemPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	//Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AFlockingSystemPlayerController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AFlockingSystemPlayerController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AFlockingSystemPlayerController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AFlockingSystemPlayerController::OnSetDestinationReleased);
	}
}

void AFlockingSystemPlayerController::OnInputStarted()
{
	StopMovement();
}

// Triggered every frame when the input is held down
void AFlockingSystemPlayerController::OnSetDestinationTriggered()
{	
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);

	UWorld* world = GetWorld();
	for (TActorIterator<AFlockingSystemCharacter> ActorItr(world); ActorItr; ++ActorItr)
	{
		AFlockingSystemCharacter * found = *ActorItr;
		AFlockAIController * controller = found->GetController<AFlockAIController>();

		// If we hit a surface, cache the location
		if (bHitSuccessful)
		{
			controller->SetGoalLocation(Hit.Location);
		}

	}



}

void AFlockingSystemPlayerController::OnSetDestinationReleased()
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
}

