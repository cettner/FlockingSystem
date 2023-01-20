// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FlockAgentGoalInterface.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFlockAgentGoalInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FLOCKINGSYSTEM_API IFlockAgentGoalInterface
{
	GENERATED_BODY()

	public:
		virtual FVector GetGoalRootLocation() const;
		/*Returns the area */
		virtual FBoxSphereBounds GetGoalBounds() const;

};
