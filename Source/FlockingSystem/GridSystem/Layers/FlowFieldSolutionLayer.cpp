// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowFieldSolutionLayer.h"

#include "SuperTileManager.h"
#include "../GameGrid.h"


void UFlowFieldSolutionLayer::AddGoalTile(UGridTile* InTile, const bool bRebuildWeights)
{
	IntegrationLayer->AddGoalTile(InTile);

	if(bRebuildWeights == true)
	{
		IntegrationLayer->RebuildWeights();
		VectorLayer->OnLayerActivate();
	}

}

void UFlowFieldSolutionLayer::AddGoalTile(TArray<UGridTile*> InTiles, const bool bRebuildWeights)
{
	IntegrationLayer->AddGoalTile(InTiles);

	if (bRebuildWeights == true)
	{
		IntegrationLayer->RebuildWeights();
		VectorLayer->OnLayerActivate();
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

bool UFlowFieldSolutionLayer::BuildSolution()
{
	bool retval = true;
	const bool bhasgoal = HasGoal();
	const bool brebuildweight = RequiresWeightRebuild();
	if (brebuildweight && bhasgoal)
	{
		IntegrationLayer->OnLayerActivate();
		bNeedsWeightRebuild = false;
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
	return bNeedsWeightRebuild;
}

bool UFlowFieldSolutionLayer::IsSolutionReady() const
{
	return HasGoal() && !bNeedsCostRebuild && !bNeedsWeightRebuild;
}

bool UFlowFieldSolutionLayer::CanUseSolutionforQuery(const FPathFindingQuery& Query) const
{
	bool retval = false;
	const AGameGrid * grid = GetGameGrid();
	const FVector endlocation = Query.EndLocation;
	const UGridTile * goaltile = grid->GetTileFromLocation(endlocation);

	if (IsGoalTile(goaltile))
	{
		retval = true;
	}

	return retval;
}

bool UFlowFieldSolutionLayer::GetFlowVectorForTile(const UGridTile* InTile, FVector& OutTile) const
{
	return VectorLayer->GetTileVector(InTile, OutTile);
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
