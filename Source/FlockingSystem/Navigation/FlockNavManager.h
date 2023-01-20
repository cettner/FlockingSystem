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

public:
	virtual void RegisterFlockAgent(IFlockAgentInterface* InAgent);
	virtual void UnRegisterFlockAgent(IFlockAgentInterface* InAgent);

	UFlock* CreateFlock(TSet<IFlockAgentInterface*> InAgents, IFlockAgentGoalInterface * InFlockGoal = nullptr);

protected:
	TSet<UObject*> GetObstacleAgents(const UFlock* InFlock) const;


protected:
	inline const TSet<UObject*>& GetAllActiveAgents() const
	{
		return AllAgents;
	}

protected:
	UPROPERTY()
	TArray<UFlock*> AllFlocks;

	UPROPERTY()
	TSet<UObject*> AllAgents;
};
