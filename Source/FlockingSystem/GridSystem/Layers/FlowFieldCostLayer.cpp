// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowFieldCostLayer.h"
#include "../GameGrid.h"


bool UFlowFieldCostLayer::GetTileCost(UGridTile* InTile, uint8& OutCost)
{
	bool retval = false;
	uint8 * costptr = CostMap.Find(InTile);
	if (costptr != nullptr)
	{
		OutCost = *costptr;
	}
	return false;
}

void UFlowFieldCostLayer::LayerInitialize(AGameGrid* InGrid)
{
	Super::LayerInitialize(InGrid);
	const TArray<UGridTile*>& alltiles = InGrid->GetTiles();

	CalculateTiles(alltiles);

}

void UFlowFieldCostLayer::PostActivateTile(UGridTile* InTile)
{
	uint8 outcost;
	if (GetTileCost(InTile, outcost) && outcost > 0U)
	{
		FLinearColor fillcolor = GetCostFillColor(outcost);
	}

}

void UFlowFieldCostLayer::CalculateTiles(const TArray<UGridTile*>& InTiles)
{
	for (int i = 0; i < InTiles.Num(); i++)
	{
		const uint8 tilecost = CalculateTileCost(InTiles[i]);
		CostMap.Emplace(InTiles[i], tilecost);
	}
}

uint8 UFlowFieldCostLayer::CalculateTileCost(const UGridTile* InTile)
{
	uint8 retval = 0U;
	UWorld* world = GetWorld();

	if (world != nullptr)
	{
		const FVector tilenormal = InTile->GetTileNormal();
		const FVector tracelocation = InTile->GetTileCenter() + (tilenormal * BlockTraceHeightOffset);
		const FCollisionShape traceshape = FCollisionShape::MakeSphere(30.0f);
		FCollisionQueryParams queryparams = FCollisionQueryParams::DefaultQueryParam;
		FName TraceTag("DebugCostTag");
		world->DebugDrawTraceTag = TraceTag;
		queryparams.TraceTag = TraceTag;
		queryparams.bTraceComplex = false;
		queryparams.bReturnFaceIndex = true;
		queryparams.bReturnPhysicalMaterial = false;
		queryparams.bFindInitialOverlaps = false;
		const FQuat tracerotation =  FRotator(0.0f, 0.0f, 0.0f).Quaternion();

		FHitResult outhit = FHitResult();
		world->SweepSingleByChannel(outhit, tracelocation, tracelocation, tracerotation, BlockTraceChannel, traceshape, queryparams);

		if (outhit.bBlockingHit)
		{
			retval = BLOCKED_TILE_COST;
		}
	}

	return retval;
}

FLinearColor UFlowFieldCostLayer::GetCostFillColor(const uint8 InCost)
{
	static const float divisor = static_cast<float>(BLOCKED_TILE_COST);
	const float costfloat = static_cast<float>(InCost);
	const float intensity = costfloat / divisor;

	const FLinearColor retval = FLinearColor(intensity, 1.0f - intensity, 0.0f, 1.0f);

	return retval;
}
