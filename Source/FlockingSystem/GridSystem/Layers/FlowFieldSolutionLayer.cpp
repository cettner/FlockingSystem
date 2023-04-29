// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowFieldSolutionLayer.h"
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
	if(RequiresCostRebuild())
	{
		CostLayer->OnLayerActivate();
		bNeedsWeightRebuild = true;
		bNeedsCostRebuild = false;
	}

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

	CostLayer = NewObject<UFlowFieldCostLayer>(InGrid, UFlowFieldCostLayer::StaticClass());
	CostLayer->SetLayerID(LayerID);
	CostLayer->LayerInitialize(InGrid, InActiveTiles, InApplicator);

	IntegrationLayer = NewObject<UFlowFieldIntegrationLayer>(InGrid, UFlowFieldIntegrationLayer::StaticClass());
	IntegrationLayer->SetLayerID(LayerID);
	IntegrationLayer->SetCostLayer(CostLayer);
	IntegrationLayer->LayerInitialize(InGrid, InActiveTiles, InApplicator);

	VectorLayer = NewObject<UFlowFieldVectorLayer>(InGrid, UFlowFieldVectorLayer::StaticClass());
	VectorLayer->SetLayerID(LayerID);
	VectorLayer->SetIntegrationLayer(IntegrationLayer);
	VectorLayer->LayerInitialize(InGrid, InActiveTiles, InApplicator);
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
