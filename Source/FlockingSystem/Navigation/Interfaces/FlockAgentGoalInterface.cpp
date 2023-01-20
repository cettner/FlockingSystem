// Fill out your copyright notice in the Description page of Project Settings.


#include "FlockAgentGoalInterface.h"

// Add default functionality here for any IFlockAgentGoalInterface functions that are not pure virtual.

FVector IFlockAgentGoalInterface::GetGoalRootLocation() const
{
    return FVector();
}

FBoxSphereBounds IFlockAgentGoalInterface::GetGoalBounds() const
{
    return FBoxSphereBounds();
}
