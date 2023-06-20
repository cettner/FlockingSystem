// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowFieldSolutionLayer.h"

#include "SuperTileManager.h"
#include "../Navigation/VectorFieldNavigationSystem.h"
#include "../GameGrid.h"


void UFlowFieldSolutionLayer::SetGoalTile(const UGridTile* InTile, const bool bRebuildWeights)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("GoalTile Set Request %d"), InTile->GetTileID()));
	IntegrationLayer->SetGoalTile(InTile);
}

void UFlowFieldSolutionLayer::SetGoalLocation(const FVector& InMoveLocation, const bool bRebuildWeights)
{
	const UGridTile* goaltile = GetGameGrid()->GetTileFromLocation(InMoveLocation);

	if (IsValid(goaltile))
	{
		Goallocation = InMoveLocation;
		SetGoalTile(goaltile, bRebuildWeights);
	}

}

void UFlowFieldSolutionLayer::SetGoalActor(const AActor* InGoalActor, const bool IsDynamicGoal, const bool bRebuildWeights)
{
	if (IsValid(InGoalActor))
	{
		const UGridTile* goaltile = GetGameGrid()->GetTileFromLocation(InGoalActor->GetActorLocation());

		if (goaltile != nullptr)
		{
			GoalActor = InGoalActor;
			LastUpdatedGoalPosition = InGoalActor->GetActorLocation();
			bIsGoalDynamic = IsDynamicGoal;
			SetGoalTile(goaltile, bRebuildWeights);
		}
	}
}

void UFlowFieldSolutionLayer::SubscribeAgent(const UObject * Subscriber)
{
	Subscribers.Add(Subscriber);
}

void UFlowFieldSolutionLayer::UnSubscribeAgent(const UObject* UnSubScriber)
{
	Subscribers.Remove(UnSubScriber);

	if (Subscribers.IsEmpty())
	{
		GetGameGrid()->RemoveGridLayer(this);
	}
}

bool UFlowFieldSolutionLayer::IsAgentSubscribed(const UObject* InCheckAgent) const
{
	return Subscribers.Contains(InCheckAgent);
}

bool UFlowFieldSolutionLayer::HasGoal() const
{
	const bool retval = IntegrationLayer->DoesGoalExist();
	return retval;
}

bool UFlowFieldSolutionLayer::IsGoalTile(const UGridTile* InGridTile) const
{
	const bool retval = IntegrationLayer->IsGoalTile(InGridTile);
	return retval;
}

void UFlowFieldSolutionLayer::ResetSolution()
{
	IntegrationLayer->ResetWeights();
	VectorLayer->ResetFlowField();
}

bool UFlowFieldSolutionLayer::BuildSolution()
{
	bool retval = true;
	const bool bhasgoal = HasGoal();
	const bool brebuildweight = RequiresWeightRebuild();
	if (brebuildweight && bhasgoal)
	{
		IntegrationLayer->OnLayerActivate();
		VectorLayer->OnLayerActivate();
	}
	else
	{
		retval = false;
	}
	
	return retval;
}

bool UFlowFieldSolutionLayer::RequiresCostRebuild() const
{
	return bNeedsCostRebuild;
}

bool UFlowFieldSolutionLayer::RequiresWeightRebuild() const
{
	return IntegrationLayer->bRequiresRebuild;
}

bool UFlowFieldSolutionLayer::IsSolutionReady() const
{
	return HasGoal() && !bNeedsCostRebuild && !RequiresWeightRebuild();
}

bool UFlowFieldSolutionLayer::CanUseSolutionforQuery(const FVectorFieldQuery& Query) const
{
	bool retval = false;

	if (Query.IsGoalActor())
	{
		retval = GetGoalActor() == Query.GetGoalActor();
	}
	else
	{
		/*Todo, allow for the creation of a copy solution if the endlocations arent the exact same.*/
		const AGameGrid* grid = GetGameGrid();
		const FVector endlocation = Query.EndLocation;
		const UGridTile* goaltile = grid->GetTileFromLocation(endlocation);

		if (IsGoalTile(goaltile))
		{
			retval = true;
		}
	}

	return retval;
}

bool UFlowFieldSolutionLayer::GetFlowVectorForTile(const UGridTile* InTile, FVector& OutTile) const
{
	return VectorLayer->GetTileVector(InTile, OutTile);
}

bool UFlowFieldSolutionLayer::GetWeightForTile(const UGridTile* InTile, float& Outweight) const
{
	return IntegrationLayer->GetTileWeight(InTile, Outweight);
}

const UGridTile* UFlowFieldSolutionLayer::GetGoalTile() const
{
	return IntegrationLayer->GetGoalTile();
}

bool UFlowFieldSolutionLayer::NeedsRepath() const
{
	bool retval = false;

	if (!IsSolutionReady())
	{
		retval = true;
	}
	else if (IsGoalDynamic())
	{
		const AActor* goalactor = GetGoalActor();
		if (IsValid(goalactor))
		{
			const FVector goalposition = Cast<INavAgentInterface>(goalactor) ? Cast<INavAgentInterface>(goalactor)->GetNavAgentLocation() : goalactor->GetActorLocation();
			const float dissquared = FVector::DistSquared(goalposition, LastUpdatedGoalPosition);

			if (dissquared >= FMath::Square(RepathTetherDistance))
			{
				retval = true;
			}
		}
	}

	return retval;
}

void UFlowFieldSolutionLayer::LayerInitialize(AGameGrid* InGrid, const TArray<UGridTile*>& InActiveTiles, AActor* InApplicator)
{
	Super::LayerInitialize(InGrid, InActiveTiles, InApplicator);

	bNeedsCostRebuild = !InitializeCostData();

	IntegrationLayer = NewObject<UFlowFieldIntegrationLayer>(InGrid, UFlowFieldIntegrationLayer::StaticClass());
	IntegrationLayer->SetLayerID(LayerID);
	IntegrationLayer->SetCostData(CostMap);
	IntegrationLayer->LayerInitialize(InGrid, InActiveTiles, InApplicator);

	VectorLayer = NewObject<UFlowFieldVectorLayer>(InGrid, UFlowFieldVectorLayer::StaticClass());
	VectorLayer->SetLayerID(LayerID);
	VectorLayer->SetIntegrationLayer(IntegrationLayer);
	VectorLayer->LayerInitialize(InGrid, InActiveTiles, InApplicator);
}

void UFlowFieldSolutionLayer::OnCostLayerRebuilt(UFlowFieldCostLayer* InRebuiltLayer)
{
}

bool UFlowFieldSolutionLayer::InitializeCostData()
{
	const AGameGrid * gamegrid = GetGameGrid();
	const TArray<UFlowFieldCostLayer*> costlayers = gamegrid->GetLayersOfClass<UFlowFieldCostLayer>();

	for (int i = 0; i < costlayers.Num(); i++)
	{
		CostMap.Append(costlayers[i]->GetAllCosts());
	}

	return CostMap.Num() == ActiveTiles.Num();
}

void UFlowFieldSolutionLayer::OnLayerActivate()
{
	BuildSolution();
}

void UFlowFieldSolutionLayer::OnShowLayer()
{
	VectorLayer->SetLayerVisibility(true);
}

void UFlowFieldSolutionLayer::OnHideLayer()
{
	VectorLayer->SetLayerVisibility(false);
}
