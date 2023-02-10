// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "../GridLayer.h"
#include "FlowFieldCostLayer.h"
#include "FlowFieldIntegrationLayer.generated.h"

constexpr uint32 UNVISITED_TILE_WEIGHT = 0xFFFFU;
constexpr uint32 GOAL_TILE_WEIGHT = 0x0000U;

UCLASS()
class FLOCKINGSYSTEM_API UFlowFieldIntegrationLayer : public UGridLayer
{
	GENERATED_BODY()

protected:
	UFlowFieldIntegrationLayer();

public:
	bool GetTileWeight(const UGridTile * InTile, uint32& OutCost) const;
	void AddGoalTile(UGridTile* InTile);
	void AddGoalTile(TArray<UGridTile*> InTiles);
	bool RemoveGoalTile(UGridTile * InTile, uint32 InReplacementWeight = UNVISITED_TILE_WEIGHT, bool InRebuildifSuccessful = true);
	bool RemoveGoalTile(TArray<UGridTile*> InTiles, uint32 InReplacementWeight = UNVISITED_TILE_WEIGHT, bool InRebuildifSuccessful = true);

	bool DoesGoalExist() const;

public:
	virtual void ShowTile(UGridTile* InTile) override;
	virtual void HideTile(UGridTile* InTile) override;


protected:
	virtual void RebuildWeights();
	virtual void BuildWeights();
	virtual UFlowFieldCostLayer* GetCostField() const;

protected:
	TMap<UGridTile*, uint32> WeightMap = TMap<UGridTile*, uint32>();
	TArray<UGridTile*> GoalTiles = TArray<UGridTile*>();

	bool bRequiresRebuild = true;

};
