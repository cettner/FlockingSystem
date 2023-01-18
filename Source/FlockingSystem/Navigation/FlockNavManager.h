// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Flock.h"
#include "FlockNavManager.generated.h"

/**
 * 
 */
UCLASS()
class FLOCKINGSYSTEM_API UFlockNavManager : public UObject
{
	GENERATED_BODY()

	friend UFlock;

protected:
	inline const TArray<IFlockAgentInterface*>& GetAllActiveAgents() const
	{
		return AllAgents;
	}

protected:
	UPROPERTY()
	TArray<UFlock*> AllFlocks;

	UPROPERTY()
	TArray<IFlockAgentInterface*> AllAgents;
};
