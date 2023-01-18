// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Interfaces/FlockAgentInterface.h"
#include "Flock.generated.h"

/**
 * 
 */
UCLASS()
class UFlock : public UObject
{
	GENERATED_BODY()

	public:
		virtual void UpdateFlock();
		
		inline const TArray<IFlockAgentInterface*>& GetAgents() const
		{
			return FlockAgents;
		}

	protected:
		TArray<IFlockAgentInterface*> FlockAgents;
	
};
