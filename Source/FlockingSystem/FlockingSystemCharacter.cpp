// Copyright Epic Games, Inc. All Rights Reserved.

#include "FlockingSystemCharacter.h"
#include "../FlockingSystemGameMode.h"
#include "../Public/FlockAIController.h"

#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"

AFlockingSystemCharacter::AFlockingSystemCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AFlockAIController::StaticClass();
}

void AFlockingSystemCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (HasAuthority())
	{
		UWorld* world = GetWorld();
		AFlockingSystemGameMode* gm = world->GetAuthGameMode<AFlockingSystemGameMode>();
		if (gm != nullptr)
		{
			gm->GetFlockManager()->RegisterFlockAgent(this);
		}

	}
}

void AFlockingSystemCharacter::BeginPlay()
{
	Super::BeginPlay();
	AFlockAIController* aic = GetController<AFlockAIController>();

	if (aic)
	{
		aic->SetGoalActor(PreSetFlockGoal);
	}
}

void AFlockingSystemCharacter::JoinFlock(UFlock* InFlock)
{
	IFlockAgentInterface::JoinFlock(InFlock);
	AFlockAIController* aic = GetController<AFlockAIController>();

	if (aic)
	{
		aic->SetGoalActor(Cast<AActor>(InFlock->GetFlockGoal()));
	}
}
