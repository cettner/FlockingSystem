// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BehaviorTree.h"

#include "FlockingSystemCharacter.generated.h"

UCLASS(Blueprintable)
class AFlockingSystemCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFlockingSystemCharacter();

protected:
	virtual void PostInitializeComponents() override;

public:
	UPROPERTY(EditDefaultsOnly, Category = Behavior)
	UBehaviorTree* BehaviorTree = nullptr;

};

