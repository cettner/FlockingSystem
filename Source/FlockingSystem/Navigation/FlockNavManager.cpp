// Fill out your copyright notice in the Description page of Project Settings.


#include "FlockNavManager.h"

/*
void UFlockNavManager::RegisterFlockAgent(IFlockAgentInterface* InAgent)
{
	FSetElementId id = AllAgents.Add(CastChecked<UObject>(InAgent));
	InAgent->SetFlockAgentID(id);
}

void UFlockNavManager::UnRegisterFlockAgent(IFlockAgentInterface* InAgent)
{
	if (InAgent != nullptr)
	{
		InAgent->LeaveFlock();
		InAgent->SetFlockAgentID(FSetElementId());
		UObject* objagent = CastChecked<UObject>(InAgent);
		AllAgents.Remove(objagent);
	}
}

UFlock* UFlockNavManager::CreateFlock(TSet<IFlockAgentInterface*> InAgents, IFlockAgentGoalInterface * InFlockGoal)
{
	UFlock* retval = nullptr;
	if (InAgents.Num() > 0)
	{
		retval = NewObject<UFlock>(this);
		retval->InitFlock(InAgents, InFlockGoal);
	}

	if (retval != nullptr)
	{
		AllFlocks.Emplace(retval);
	}

	return retval;
}

TSet<UObject*> UFlockNavManager::GetObstacleAgents(const UFlock* InFlock) const
{
	const TSet<UObject*>& flockagents = InFlock->GetAgents();
	TSet<UObject*> otheragents = AllAgents.Difference(flockagents);

	return otheragents;
}
*/