// Fill out your copyright notice in the Description page of Project Settings.


#include "Flock.h"

void UFlock::UpdateFlock()
{
	for (int i = 0; i < FlockAgents.Num(); i++)
	{
		IFlockAgentInterface* agent = FlockAgents[i];

		agent->CalcSeparation();
		agent->CalcAlignment();
		agent->CalcCohesion();

		// Apply obstacle avoidance
		//agent->ApplyObstacleAvoidance();

	}
}
