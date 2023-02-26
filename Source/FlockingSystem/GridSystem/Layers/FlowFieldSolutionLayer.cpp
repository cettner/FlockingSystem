// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowFieldSolutionLayer.h"
#include "../GameGrid.h"

UFlowFieldSolutionLayer::UFlowFieldSolutionLayer() : Super()
{
	bActivatesOnStartup = false;
}

void UFlowFieldSolutionLayer::AddGoalTile(UGridTile* InTile, const bool bRebuildWeights)
{
	IntegrationLayer->AddGoalTile(InTile);

	if(bRebuildWeights == true)
	{
		IntegrationLayer->RebuildWeights();
		VectorLayer->OnLayerActivate(ActiveTiles);
	}

}

void UFlowFieldSolutionLayer::AddGoalTile(TArray<UGridTile*> InTiles, const bool bRebuildWeights)
{
	IntegrationLayer->AddGoalTile(InTiles);

	if (bRebuildWeights == true)
	{
		IntegrationLayer->RebuildWeights();
		VectorLayer->OnLayerActivate(ActiveTiles);
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
		CostLayer->OnLayerActivate(ActiveTiles);
		bNeedsWeightRebuild = true;
		bNeedsCostRebuild = false;
	}

	const bool bhasgoal = HasGoal();
	const bool brebuildweight = RequiresWeightRebuild();
	if (brebuildweight && bhasgoal)
	{
		IntegrationLayer->OnLayerActivate(ActiveTiles);
		bNeedsWeightRebuild = false;
		VectorLayer->OnLayerActivate(ActiveTiles);
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
	return false;
}

bool UFlowFieldSolutionLayer::GetFlowVectorForTile(const UGridTile* InTile, FVector& OutTile) const
{
	return VectorLayer->GetTileVector(InTile, OutTile);
}

void UFlowFieldSolutionLayer::LayerInitialize(AGameGrid* InGrid)
{
	Super::LayerInitialize(InGrid);

	CostLayer = NewObject<UFlowFieldCostLayer>(InGrid, UFlowFieldCostLayer::StaticClass());
	CostLayer->SetLayerID(LayerID);
	CostLayer->LayerInitialize(InGrid);

	IntegrationLayer = NewObject<UFlowFieldIntegrationLayer>(InGrid, UFlowFieldIntegrationLayer::StaticClass());
	IntegrationLayer->SetLayerID(LayerID);
	IntegrationLayer->SetCostLayer(CostLayer);
	IntegrationLayer->LayerInitialize(InGrid);

	VectorLayer = NewObject<UFlowFieldVectorLayer>(InGrid, UFlowFieldVectorLayer::StaticClass());
	VectorLayer->SetLayerID(LayerID);
	VectorLayer->SetIntegrationLayer(IntegrationLayer);
	VectorLayer->LayerInitialize(InGrid);
}

void UFlowFieldSolutionLayer::OnLayerActivate(TArray<UGridTile*> TileSubset)
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
