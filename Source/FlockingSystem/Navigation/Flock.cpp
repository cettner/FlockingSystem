// Fill out your copyright notice in the Description page of Project Settings.


#include "Flock.h"
#include "FlockNavManager.h"

void UFlock::InitFlock(TSet<IFlockAgentInterface*> InAgents, IFlockAgentGoalInterface* AgentGoal)
{
	if (FlockAgents.Num())
	{
		ClearFlock();
	}
	for (IFlockAgentInterface*& Elem :  InAgents)
	{
		if (Elem != nullptr)
		{
			if (Elem->GetFlock() != nullptr)
			{
				Elem->LeaveFlock();
			}

			AddAgent(Elem); 
		}

	}
}

void UFlock::UpdateFlock()
{
	for (UObject*& Elem : FlockAgents)
	{
		IFlockAgentInterface* agent = CastChecked<IFlockAgentInterface>(Elem);
		agent->UpdateFlockParameters();
	}
}

void UFlock::ClearFlock()
{
	for (UObject*& Elem : FlockAgents)
	{
		IFlockAgentInterface* agent = CastChecked<IFlockAgentInterface>(Elem);
		agent->LeaveFlock();
	}
}

void UFlock::AddAgent(IFlockAgentInterface* InAgent)
{
	FSetElementId agentid = FlockAgents.Add(Cast<UObject>(InAgent));
	InAgent->SetFlock(this);
}

bool UFlock::RemoveAgent(IFlockAgentInterface* InAgent)
{
	bool retval = FlockAgents.Remove(Cast<UObject>(InAgent)) > 0;
	InAgent->SetFlock(nullptr);
	return retval;
}

TSet<UObject*> UFlock::GetFlockDynamicObstacles() const
{
	return 	Cast<UFlockNavManager>(GetOuter())->GetObstacleAgents(this);
}

bool UFlock::GetFlockGoalLocation(FVector& OutGoalLocation) const
{
	bool retval = false;
	if (IFlockAgentGoalInterface * goal = GetFlockGoal())
	{
		OutGoalLocation = goal->GetGoalRootLocation();
		retval = true;
	}

	return retval;
}
