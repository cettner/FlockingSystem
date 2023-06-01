// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "../GridLayer.h"
#include "FlowFieldCostLayer.h"
#include "FlowFieldIntegrationLayer.generated.h"

constexpr float UNVISITED_TILE_WEIGHT = MAX_FLT / 2.0f;
constexpr float GOAL_TILE_WEIGHT = 0.0f;

UCLASS()
class FLOCKINGSYSTEM_API UFlowFieldIntegrationLayer : public UGridLayer
{
	GENERATED_BODY()

	friend class UFlowFieldSolutionLayer;

protected:

public:
	FORCEINLINE bool GetTileWeight(const UGridTile * InTile, float& OutCost) const;
	void AddGoalTile(UGridTile* InTile);
	void AddGoalTile(TArray<UGridTile*> InTiles);
	FORCEINLINE bool IsGoalTile(const UGridTile* InTile) const { return GoalTiles.Contains(InTile); };
	bool RemoveGoalTile(UGridTile * InTile, float InReplacementWeight = UNVISITED_TILE_WEIGHT, bool InRebuildifSuccessful = true);
	bool RemoveGoalTile(TArray<UGridTile*> InTiles, float InReplacementWeight = UNVISITED_TILE_WEIGHT, bool InRebuildifSuccessful = true);

	bool DoesGoalExist() const;

public:
	virtual void OnLayerActivate() override;
	virtual void ShowTile(UGridTile* InTile) override;
	virtual void HideTile(UGridTile* InTile) override;


protected:
	virtual void RebuildWeights();
	virtual void BuildWeights();
	virtual void SetCostData(const TMap<UGridTile*, uint8>& InCostMap);
	virtual bool GetTileCost(const UGridTile* InTile, uint8& OutCost) const;

protected:
	TMap<UGridTile*, float> WeightMap = TMap<UGridTile*, float>();
	TSet<UGridTile*> GoalTiles = TSet<UGridTile*>();

	bool bRequiresRebuild = true;

	const TMap<UGridTile*, uint8> *  CostMapRef = nullptr;

};
