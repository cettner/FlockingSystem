// Fill out your copyright notice in the Description page of Project Settings.


#include "FlockAgentInterface.h"
#include "../Flock.h"
// Add default functionality here for any IRTSFlockAgentInterface functions that are not pure virtual.

void IFlockAgentInterface::JoinFlock(UFlock* InFlock)
{
	InFlock->AddAgent(this);
}

void IFlockAgentInterface::LeaveFlock()
{
	UFlock* currentflock = GetFlock();
	
	if (IsValid(currentflock))
	{
		currentflock->RemoveAgent(this);
	}
}

FVector IFlockAgentInterface::GetFlockAgentLocation() const
{
	return FVector();
}

FVector IFlockAgentInterface::GetFlockAgentDirection() const
{
	return FVector();
}

float IFlockAgentInterface::GetFlockAgentRadius() const
{
	return 0.0f;
}

void IFlockAgentInterface::UpdateFlockParameters()
{
	FlockSeparation = CalcSeparation();
	FlockAlignment = CalcAlignment();
	FlockCohesion = CalcCohesion();
}

FVector IFlockAgentInterface::CalcSeparation()
{
	UFlock* flock = GetFlock();
	FVector retval = FVector::ZeroVector;

	if (flock != nullptr)
	{
		const TSet<UObject*> flockmembers = flock->GetAgents();

		for (const UObject* const & Elem : flockmembers)
		{
			const IFlockAgentInterface* otheragent = CastChecked<IFlockAgentInterface>(Elem);
			if (otheragent != this)
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
		const TSet<UObject*> flockmembers = flock->GetAgents();

		for (const UObject* const& Elem : flockmembers)
		{
			const IFlockAgentInterface* otheragent = CastChecked<IFlockAgentInterface>(Elem);
			
			if (otheragent != this)
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
		const TSet<UObject*> flockmembers = flock->GetAgents();

		for (const UObject* const& Elem : flockmembers)
		{
			const IFlockAgentInterface* otheragent = CastChecked<IFlockAgentInterface>(Elem);
			if (otheragent != this)
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

