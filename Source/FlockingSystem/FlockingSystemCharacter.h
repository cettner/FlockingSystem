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

protected:
	virtual void PostInitializeComponents() override;

protected:
	virtual void JoinFlock(UFlock* InFlock) override;

public:
	UPROPERTY(EditDefaultsOnly, Category = Behavior)
	UBehaviorTree* BehaviorTree = nullptr;

};

