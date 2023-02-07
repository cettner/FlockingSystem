// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowFieldCostLayer.h"
#include "../GameGrid.h"


void UFlowFieldCostLayer::LayerInitialize(AGameGrid* InGrid)
{
	Super::LayerInitialize(InGrid);
	const TArray<UGridTile*>& alltiles = InGrid->GetTiles();

	for (int i = 0; i < alltiles.Num(); i++)
	{
		const uint8 tilecost = CalculateTileCost(alltiles[i]);
		CostMap.Emplace(alltiles[i], tilecost);
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
