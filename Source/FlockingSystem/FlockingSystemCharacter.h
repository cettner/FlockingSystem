// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BehaviorTree.h"

#include "Navigation/Interfaces/FlockAgentGoalInterface.h"
#include "Navigation/Interfaces/FlockAgentInterface.h"
#include "FlockingSystemCharacter.generated.h"

UCLASS(Blueprintable)
class AFlockingSystemCharacter : public ACharacter, public IFlockAgentInterface
{
	GENERATED_BODY()

public:
	AFlockingSystemCharacter();

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

protected:
	virtual void JoinFlock(UFlock* InFlock) override;

public:
	UPROPERTY(EditAnywhere, transient)
	AActor * PreSetFlockGoal = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = Behavior)
	UBehaviorTree* BehaviorTree = nullptr;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
};

