// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../GridLayer.h"
#include "FlowFieldIntegrationLayer.generated.h"

constexpr uint8 UNVISITED_TILE_WEIGHT = 0xFFU;
constexpr uint8 GOAL_TILE_WEIGHT = 0x00U;

UCLASS()
class FLOCKINGSYSTEM_API UFlowFieldIntegrationLayer : public UGridLayer
{
	GENERATED_BODY()

protected:
	UFlowFieldIntegrationLayer();

public:
	void SetGoalTile(UGridTile* InTile);
	void SetGoalTile(TArray<UGridTile*> InTiles);
	bool RemoveGoalTile(UGridTile * InTile, uint8 InReplacementWeight = UNVISITED_TILE_WEIGHT, bool InRebuildifSuccessful = true);
	bool RemoveGoalTile(TArray<UGridTile*> InTiles, uint8 InReplacementWeight = UNVISITED_TILE_WEIGHT, bool InRebuildifSuccessful = true);

	bool DoesGoalExist() const;
	
protected:
	virtual void RebuildWeights();


protected:
	TMap<UGridTile*, uint8> WeightMap = TMap<UGridTile*, uint8>();
	
	bool bRequiresRebuild = true;

};
