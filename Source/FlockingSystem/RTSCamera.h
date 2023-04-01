// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/Pawn.h"

#include "Camera/RTSCameraInputConfigData.h"
#include "RTSCamera.generated.h"

UCLASS()
class FLOCKINGSYSTEM_API ARTSCamera : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARTSCamera();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
protected:

	void MoveCamera(const FInputActionValue& Value);

	//Camera Movement Functions
	void TranslateCamLeftRight(float AxisVal);
	void TranslateCamUpDown(float AxisVal);
	void ZoomIn();
	void ZoomOut();




	USpringArmComponent* MainCameraSpringArm;
	UCameraComponent* MainCamera;





protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	URTSCameraInputConfigData* InputData;

	/** MappingContext */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float CameraSpeed = 800.0f;

protected:
	FVector CameraMove = FVector::ZeroVector;
};
