// Fill out your copyright notice in the Description page of Project Settings.


#include "FlowFieldIntegrationLayer.h"

UFlowFieldIntegrationLayer::UFlowFieldIntegrationLayer()
{
	bActivatesOnStartup = false;
}

void UFlowFieldIntegrationLayer::SetGoalTile(UGridTile* InTile)
{
	WeightMap.Emplace(InTile, GOAL_TILE_WEIGHT);
}

void UFlowFieldIntegrationLayer::SetGoalTile(TArray<UGridTile*> InTiles)
{

}

bool UFlowFieldIntegrationLayer::RemoveGoalTile(UGridTile* InTile, uint8 InReplacementWeight, bool InRebuildifSuccessful)
{
	return false;
}

bool UFlowFieldIntegrationLayer::RemoveGoalTile(TArray<UGridTile*> InTiles, uint8 InReplacementWeight, bool InRebuildifSuccessful)
{
	return false;
}

bool UFlowFieldIntegrationLayer::DoesGoalExist() const
{

	return false;
}

void UFlowFieldIntegrationLayer::RebuildWeights()
{
}
