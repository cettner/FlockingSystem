// Fill out your copyright notice in the Description page of Project Settings.


#include "FlockAgentInterface.h"
#include "../Flock.h"
// Add default functionality here for any IRTSFlockAgentInterface functions that are not pure virtual.

FVector IFlockAgentInterface::GetFlockAgentLocation() const
{
	return FVector();
}

FVector IFlockAgentInterface::GetFlockAgentDirection() const
{
	return FVector();
}

FVector IFlockAgentInterface::CalcSeparation()
{
	UFlock* flock = GetFlock();
	FVector retval = FVector::ZeroVector;

	if (flock != nullptr)
	{
		const TArray<IFlockAgentInterface*> flockmembers = flock->GetAgents();

		for (int i = 0; i < flockmembers.Num(); i++)
		{
			const IFlockAgentInterface* otheragent = flockmembers[i];
			if (flockmembers[i] != this)
			{
				const FVector otherposition = otheragent->GetFlockAgentLocation();
				const FVector myposition = GetFlockAgentLocation();
				const FVector direction = (otherposition - myposition).GetSafeNormal();
				const float distance = FVector::Distance(otherposition, myposition);

				retval += direction / distance;
			}
		}
	}
	return retval;
}

FVector IFlockAgentInterface::CalcAlignment()
{
	UFlock* flock = GetFlock();
	FVector retval = FVector::ZeroVector;

	if (flock != nullptr)
	{
		const TArray<IFlockAgentInterface*> flockmembers = flock->GetAgents();

		for (int i = 0; i < flockmembers.Num(); i++)
		{
			const IFlockAgentInterface* otheragent = flockmembers[i];
			if (flockmembers[i] != this)
			{
				retval += otheragent->GetFlockAgentDirection();
			}
		}

		retval /= flockmembers.Num();
		retval.Normalize();
	}

	return retval;
}

FVector IFlockAgentInterface::CalcCohesion()
{
	FVector retval = FVector::ZeroVector;
	UFlock* flock = GetFlock();

	if (flock != nullptr)
	{
		const TArray<IFlockAgentInterface*> flockmembers = flock->GetAgents();

		for (int i = 0; i < flockmembers.Num(); i++)
		{
			const IFlockAgentInterface* otheragent = flockmembers[i];
			if (flockmembers[i] != this)
			{
				retval += otheragent->GetFlockAgentLocation();
			}
		}

		retval /= flockmembers.Num();

		/*Calculate the direction towards the "center of mass" of the flock*/
		retval = (retval - GetFlockAgentDirection()).GetSafeNormal();
	}

	return retval;
}

