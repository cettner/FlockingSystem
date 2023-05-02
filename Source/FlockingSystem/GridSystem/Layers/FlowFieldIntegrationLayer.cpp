// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowFieldIntegrationLayer.h"
#include "../GameGrid.h"



bool UFlowFieldIntegrationLayer::GetTileWeight(const UGridTile* InTile, float& OutCost) const
{
	bool retval = false;
	const float* costptr = WeightMap.Find(InTile);
	if (costptr != nullptr)
	{
		OutCost = *costptr;
		retval = true;
	}
	return retval;
}

void UFlowFieldIntegrationLayer::AddGoalTile(UGridTile* InTile)
{
	GoalTiles.Emplace(InTile);
	WeightMap.Emplace(InTile, GOAL_TILE_WEIGHT);
}

void UFlowFieldIntegrationLayer::AddGoalTile(TArray<UGridTile*> InTiles)
{
	for (int32 i = 0; i < InTiles.Num(); i++)
	{
		AddGoalTile(InTiles[i]);
	}
}

bool UFlowFieldIntegrationLayer::RemoveGoalTile(UGridTile* InTile, float InReplacementWeight, bool InRebuildifSuccessful)
{
	return false;
}

bool UFlowFieldIntegrationLayer::RemoveGoalTile(TArray<UGridTile*> InTiles, float InReplacementWeight, bool InRebuildifSuccessful)
{
	return false;
}

bool UFlowFieldIntegrationLayer::DoesGoalExist() const
{
	const bool retval = GoalTiles.Num() > 0;
	return retval;
}

void UFlowFieldIntegrationLayer::OnLayerActivate()
{
	Super::OnLayerActivate();
	BuildWeights();
}

void UFlowFieldIntegrationLayer::ShowTile(UGridTile* InTile)
{
	float tileweight;
	const FVector tilecenter = InTile->GetTileCenter();
	const int32 tileid = InTile->GetTileID();
	if (GetTileWeight(InTile, tileweight))
	{
		const FString idstring = "ID : " + FString::FromInt(tileid) + "\n";
		FString weightstring;

		if (tileweight >= UNVISITED_TILE_WEIGHT)
		{
			weightstring = "Weight : MAX";
		}
		else
		{
			weightstring = "Weight : " + FString::SanitizeFloat(tileweight, 2);
		}

		const FString debugstring = idstring + weightstring;

		DrawDebugString(GetWorld(), tilecenter, debugstring, GetGameGrid(), FColor::Magenta);
	}
	else
	{
		DrawDebugString(GetWorld(), tilecenter, "ERROR", GetGameGrid(), FColor::Red);
	}

}

void UFlowFieldIntegrationLayer::HideTile(UGridTile* InTile)
{
	FlushDebugStrings(GetWorld());
}

void UFlowFieldIntegrationLayer::RebuildWeights()
{
	for (int i = 0; i < ActiveTiles.Num(); i++)
	{
		WeightMap.Emplace(ActiveTiles[i], UNVISITED_TILE_WEIGHT);
	}
	for (UGridTile*& goaltile : GoalTiles)
	{
		WeightMap.Emplace(goaltile, GOAL_TILE_WEIGHT);
	}
}

void UFlowFieldIntegrationLayer::BuildWeights()
{
	/*set the value of all tiles except the goal(s) to max / unexplored value*/
	RebuildWeights();
	if (DoesGoalExist())
	{
		/*create a list of tiles that need to be explored, starting at the goals and branching out as the algorithm extends*/
		TQueue<UGridTile*> openlist = TQueue<UGridTile*>();
		/*The Queue class doesnt support internal retrival so this set lets us check whats in openlist*/
		TSet<UGridTile*> visitedlist = TSet<UGridTile*>(GoalTiles);
		for (UGridTile *& goaltile : GoalTiles)
		{
			openlist.Enqueue(goaltile);
		}

		UGridTile* roottile;
		/*While Tiles need to be explored, get the one at the front of the list*/
		while (openlist.Dequeue(roottile))
		{
			visitedlist.Remove(roottile);
			TArray<FGridTileNeighbor> neighbors = roottile->GetNeighbors();

			for (int i = 0; i < neighbors.Num(); i++)
			{
				UGridTile*& neighbor = neighbors[i].NeighborTile;
				float rootweight;
				float neighborweight; 
				uint8 neighborcost;
				float neighborcostfloat;

				/*Security Checking, make sure that the tiles being aquired are valid and costs have been computed  correctly*/
				const bool rootweightsuccess = GetTileWeight(roottile, rootweight);
				const bool neighborweightsuccess = GetTileWeight(neighbor, neighborweight);
				const bool neighborcostsuccess = GetTileCost(neighbor, neighborcost);
				const bool tilecheck = neighborweightsuccess && rootweightsuccess && neighborcostsuccess;

				neighborcostfloat = static_cast<float>(neighborcost);
				if (neighbors[i].bIsCornerNeighbor == true)
				{
					neighborweight += static_cast<float>(DOUBLE_UE_SQRT_2);
					neighborcostfloat += static_cast<float>(DOUBLE_UE_SQRT_2);
				}

				checkf(tilecheck, TEXT("UFlowFieldIntegrationLayer::BuildWeights : Invalid Tile retrieval for layer"))
				/*Skip any neighbor that is blocked*/
				if (neighborcost != BLOCKED_TILE_COST)
				{
					/*Compute the totalcost of the trip from the explored tile to the neighbor tile/*/
					const float totalcost = rootweight + neighborcostfloat;
					/*If the tile cost is lower than the current explored weight set the value*/
					if (totalcost < neighborweight)
					{
						/*if we're already going to explore it dont add it again*/
						if (!visitedlist.Contains(neighbor))
						{
							openlist.Enqueue(neighbor);
							visitedlist.Emplace(neighbor);
						}
						/*Assign the new Weight*/
						WeightMap.Emplace(neighbor, totalcost);
					}
				}
			}
		}
	}
}

void UFlowFieldIntegrationLayer::SetCostData(const TMap<UGridTile*, uint8>& InCostMap)
{
	CostMapRef = &InCostMap;
}

bool UFlowFieldIntegrationLayer::GetTileCost(const UGridTile* InTile, uint8& OutCost) const
{
	checkf(CostMapRef, TEXT("UFlowFieldIntegrationLayer::GetTileCost CostMapRef was null"))
	bool retval = false;
	
	const uint8* costptr = CostMapRef->Find(InTile);
	if (costptr != nullptr)
	{
		OutCost = *costptr;
		retval = true;
	}

	return retval;
}
