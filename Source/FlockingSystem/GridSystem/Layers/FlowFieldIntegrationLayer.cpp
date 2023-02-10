// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowFieldIntegrationLayer.h"
#include "../GameGrid.h"

UFlowFieldIntegrationLayer::UFlowFieldIntegrationLayer()
{
	bActivatesOnStartup = false;
}

bool UFlowFieldIntegrationLayer::GetTileWeight(const UGridTile* InTile, uint32& OutCost) const
{
	bool retval = false;
	const uint32* costptr = WeightMap.Find(InTile);
	if (costptr != nullptr)
	{
		OutCost = *costptr;
		retval = true;
	}
	return retval;
}

void UFlowFieldIntegrationLayer::AddGoalTile(UGridTile* InTile)
{
	GoalTiles.AddUnique(InTile);
	WeightMap.Emplace(InTile, GOAL_TILE_WEIGHT);
}

void UFlowFieldIntegrationLayer::AddGoalTile(TArray<UGridTile*> InTiles)
{
	for (int32 i = 0; i < InTiles.Num(); i++)
	{
		AddGoalTile(InTiles[i]);
	}
}

bool UFlowFieldIntegrationLayer::RemoveGoalTile(UGridTile* InTile, uint32 InReplacementWeight, bool InRebuildifSuccessful)
{
	return false;
}

bool UFlowFieldIntegrationLayer::RemoveGoalTile(TArray<UGridTile*> InTiles, uint32 InReplacementWeight, bool InRebuildifSuccessful)
{
	return false;
}

bool UFlowFieldIntegrationLayer::DoesGoalExist() const
{
	const bool retval = GoalTiles.Num() > 0;
	return retval;
}

void UFlowFieldIntegrationLayer::ShowTile(UGridTile* InTile)
{
	uint32 tileweight;
	const FVector tilecenter = InTile->GetTileCenter();
	if (GetTileWeight(InTile, tileweight))
	{
		DrawDebugString(GetWorld(), tilecenter, FString::FromInt(tileweight), GetGameGrid(), FColor::Green);
	}
	else
	{
		DrawDebugString(GetWorld(), tilecenter, "ERROR", GetGameGrid(), FColor::Red);
	}

}

void UFlowFieldIntegrationLayer::HideTile(UGridTile* InTile)
{

}

void UFlowFieldIntegrationLayer::RebuildWeights()
{
	for (int i = 0; i < ActiveTiles.Num(); i++)
	{
		WeightMap.Emplace(ActiveTiles[i], UNVISITED_TILE_WEIGHT);
	}
	for (int i = 0; i < GoalTiles.Num(); i++)
	{
		WeightMap.Emplace(GoalTiles[i], GOAL_TILE_WEIGHT);
	}
}

void UFlowFieldIntegrationLayer::BuildWeights()
{
	RebuildWeights();
	TArray<UGridTile*> openlist = GoalTiles;
	if (DoesGoalExist())
	{
		UFlowFieldCostLayer* costfield = GetCostField();
		while (openlist.Num() > 0)
		{
			UGridTile* firsttile = openlist.Pop(false);
			TArray<UGridTile*> neighbors = firsttile->GetNeighbors();

			for (int i = 0; i < neighbors.Num(); i++)
			{
				UGridTile*& neighbor = neighbors[i];
				uint32 rootweight;
				uint32 neighborweight; 
				uint8 neighborcost;

				const bool rootweightsuccess = GetTileWeight(firsttile, rootweight);
				const bool neighborweightsuccess = GetTileWeight(neighbor, neighborweight);
				const bool neighborcostsuccess = costfield->GetTileCost(neighbor, neighborcost);
				if (neighborweightsuccess && rootweightsuccess && neighborcostsuccess)
				{
					uint32 totalcost = rootweight + static_cast<uint32>(neighborcost);
					if (totalcost < neighborweight)
					{
						openlist.AddUnique(neighbor);
					}

					WeightMap.Emplace(GoalTiles[i], totalcost);
				}
				else
				{
					//todo log out failure....
				}
 
			}
		}
	}
}

UFlowFieldCostLayer* UFlowFieldIntegrationLayer::GetCostField() const
{
	return GetGameGrid()->GetLayerOfClass<UFlowFieldCostLayer>();
}
