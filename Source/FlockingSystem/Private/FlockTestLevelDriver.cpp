// Fill out your copyright notice in the Description page of Project Settings.


#include "FlockTestLevelDriver.h"
#include "../FlockingSystemGameMode.h"
#include "../FlockingSystemCharacter.h"

#include "EngineUtils.h"

void AFlockTestLevelDriver::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		TMultiMap<UObject*, AFlockingSystemCharacter*> goalmap = TMultiMap<UObject*, AFlockingSystemCharacter*>();
		UWorld* world = GetWorld();
		AFlockingSystemGameMode * gm = world->GetAuthGameMode<AFlockingSystemGameMode>();

		for (TObjectIterator<AFlockingSystemCharacter> Itr; Itr; ++Itr)
		{
			AFlockingSystemCharacter* character = *Itr;
			AActor* goalactor = character->PreSetFlockGoal;
			if (IsValid(goalactor))
			{
				goalmap.Emplace(goalactor, character);
			}
		}

		TSet<UObject*> keys = TSet<UObject*>();
		goalmap.GetKeys(keys);

		for (auto& Elem : keys)
		{
			TArray<AFlockingSystemCharacter*> flock = TArray<AFlockingSystemCharacter*>();
			goalmap.MultiFind(Elem, flock);

			TSet<IFlockAgentInterface*> flockset = TSet<IFlockAgentInterface*>();
			for (auto& flockmate : flock)
			{
				flockset.Emplace(flockmate);
			}

			gm->GetFlockManager()->CreateFlock(flockset, Cast<IFlockAgentGoalInterface>(Elem));
		}
	}



}
