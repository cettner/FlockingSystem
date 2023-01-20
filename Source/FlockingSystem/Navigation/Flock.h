// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Interfaces/FlockAgentGoalInterface.h"
#include "Interfaces/FlockAgentInterface.h"
#include "Flock.generated.h"



UCLASS()
class UFlock : public UObject
{
	GENERATED_BODY()

	public:
		virtual void InitFlock(TSet<IFlockAgentInterface*> InAgents, IFlockAgentGoalInterface * AgentGoal = nullptr);
		virtual void UpdateFlock();
		virtual void ClearFlock();
		virtual IFlockAgentGoalInterface* GetFlockGoal() const { return FlockGoal; }

		virtual void AddAgent(IFlockAgentInterface* InAgent);
		virtual bool RemoveAgent(IFlockAgentInterface* InAgent);

		TSet<UObject*> GetFlockDynamicObstacles() const;

		/*Returns the root Location of the Goal Object, if it exists*/
		virtual bool GetFlockGoalLocation(FVector &OutGoalLocation) const;

		inline const TSet<UObject*>& GetAgents() const
		{
			return FlockAgents;
		}

	protected:
		TSet<UObject*> FlockAgents;

		IFlockAgentGoalInterface* FlockGoal = nullptr;
	
};
