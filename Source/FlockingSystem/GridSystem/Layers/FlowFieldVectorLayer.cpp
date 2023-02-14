// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowFieldVectorLayer.h"
#include "../GameGrid.h"

#include "DrawDebugHelpers.h"

UFlowFieldVectorLayer::UFlowFieldVectorLayer()
{
	bActivatesOnStartup = false;
}

void UFlowFieldVectorLayer::PostActivateTile(UGridTile* InTile)
{
	const UFlowFieldIntegrationLayer* integrationlayer = GetIntegrationLayer();
	
	if (!integrationlayer->IsGoalTile(InTile))
	{
		const TArray<FGridTileNeighbor> neighbors = InTile->GetNeighbors();

		float runningmin = UNVISITED_TILE_WEIGHT;
		float currentweight;
		UGridTile* runningtile = nullptr;
		for (int i = 0; i < neighbors.Num(); i++)
		{
			UGridTile* currentneighbor = neighbors[i].NeighborTile;
			integrationlayer->GetTileWeight(currentneighbor, currentweight);
			if (currentweight < runningmin)
			{
				runningmin = currentweight;
				runningtile = currentneighbor;
			}
		}

		/*Can Occur if surounding tiles are blocked / unvisited*/
		if (runningtile != nullptr)
		{
			const FVector intilevector = InTile->GetTileCenter();
			const FVector neighborvector = runningtile->GetTileCenter();
			FVector returnvector = neighborvector - intilevector;
			returnvector.Normalize();

			FlowMap.Emplace(InTile, returnvector);
		}
	}
}

void UFlowFieldVectorLayer::ShowTile(UGridTile* InTile)
{
	FVector flowvector;

	if (GetTileVector(InTile, flowvector))
	{
		FVector tilecenter = InTile->GetTileCenter();
		float arrowsize = 500.0f;
		FVector endpoint = tilecenter + (flowvector * 70.0f);

		DrawDebugDirectionalArrow(GetWorld(),tilecenter, endpoint, arrowsize, FColor::Black, true, -1.0f, 0U, 10.0f);
	}


}

void UFlowFieldVectorLayer::HideTile(UGridTile* InTile)
{

}

bool UFlowFieldVectorLayer::GetTileVector(const UGridTile* InTile, FVector& OutVector) const
{
	bool retval = false;
	const FVector* costptr = FlowMap.Find(InTile);
	if (costptr != nullptr)
	{
		OutVector = *costptr;
		retval = true;
	}
	return retval;
}

UFlowFieldIntegrationLayer* UFlowFieldVectorLayer::GetIntegrationLayer() const
{
	return IntegrationLayer;
}

void UFlowFieldVectorLayer::SetIntegrationLayer(UFlowFieldIntegrationLayer* InIntegrationLayer)
{
	IntegrationLayer = InIntegrationLayer;
}
