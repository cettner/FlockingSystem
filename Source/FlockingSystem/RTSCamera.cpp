// Fill out your copyright notice in the Description page of Project Settings.

#include "RTSCamera.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


// Sets default values
ARTSCamera::ARTSCamera()
{
	// Camera is invisible and only manipulates the game through commands to AI units which are server controlled, so the actual position of the camera doesnt matter to the server / client
	bReplicates = false;
	
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = !(GetNetMode() == NM_DedicatedServer && GetLocalRole() == ROLE_Authority);
	
	// Because the camera is tilted relative to the direction we want to move we create a scenecomponent to maintain the sense of "forward"
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	
	// Attach and tilt the camera downwards
	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	MainCamera->SetupAttachment(RootComponent);
	MainCamera->SetRelativeRotation(FRotator(-45.0, 0.0, 0.0));

}

void ARTSCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CameraMove.IsZero())
	{
		CameraMove = CameraMove.GetSafeNormal() * CameraSpeed;
		FVector NewLocal = GetActorLocation();
		FVector XYPlane = FVector::CrossProduct(GetActorForwardVector(), FVector(0.0,0.0,-1.0));
		NewLocal += GetActorForwardVector() * CameraMove.X * DeltaTime;
		NewLocal += XYPlane * CameraMove.Y * DeltaTime;
		SetActorLocation(NewLocal);
	}

}

void ARTSCamera::MoveCamera(const FInputActionValue& Value)
{
	const FVector2D MoveValue = Value.Get<FVector2D>();
	if (MoveValue.IsZero())
	{
		CameraMove.X = FVector::ZeroVector.X;
		CameraMove.Y = FVector::ZeroVector.Y;
	}
	else
	{
		TranslateCamLeftRight(MoveValue.X);
		TranslateCamUpDown(MoveValue.Y);
	}

}

void ARTSCamera::TranslateCamLeftRight(float AxisVal)
{
	CameraMove.Y = FMath::Clamp<float>(AxisVal, -1.0, 1.0);
}

void ARTSCamera::TranslateCamUpDown(float AxisVal)
{
	CameraMove.X = FMath::Clamp<float>(AxisVal, -1.0, 1.0);
}

void ARTSCamera::ZoomIn()
{
	CameraMove = CameraMove.GetSafeNormal() * CameraSpeed;
	FVector NewLocal = GetActorLocation();
	NewLocal += MainCamera->GetForwardVector() * (CameraMove.Z + 50);
	SetActorLocation(NewLocal);
}

void ARTSCamera::ZoomOut()
{
	CameraMove = CameraMove.GetSafeNormal() * CameraSpeed;
	FVector NewLocal = GetActorLocation();
	NewLocal += MainCamera->GetForwardVector() * (CameraMove.Z - 50);
	SetActorLocation(NewLocal);
}

void ARTSCamera::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Get the player controller
	APlayerController* PC = Cast<APlayerController>(GetController());

	// Get the local player subsystem
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	Subsystem->AddMappingContext(DefaultMappingContext, 0);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(InputData->IA_Move, ETriggerEvent::Triggered, this, &ARTSCamera::MoveCamera);
		EnhancedInputComponent->BindAction(InputData->IA_Move, ETriggerEvent::Completed, this, &ARTSCamera::MoveCamera);
	}

}

