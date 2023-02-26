// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavigationSystemTypes.h"

#include "../GridLayer.h"
#include "FlowFieldCostLayer.h"
#include "FlowFieldIntegrationLayer.h"
#include "FlowFieldVectorLayer.h"
#include "FlowFieldSolutionLayer.generated.h"

/**
 * 
 */
UCLASS()
class FLOCKINGSYSTEM_API UFlowFieldSolutionLayer : public UGridLayer
{
	GENERATED_BODY()

	UFlowFieldSolutionLayer();

public:
	void AddGoalTile(UGridTile* InTile, const bool bRebuildWeights = false);
	void AddGoalTile(TArray<UGridTile*> InTiles, const bool bRebuildWeights = false);
	bool HasGoal() const;
	FORCEINLINE bool IsGoalTile(const UGridTile * InGridTile) const;
	virtual bool BuildSolution();
	virtual bool RequiresCostRebuild() const;
	virtual bool RequiresWeightRebuild() const;
	virtual bool IsSolutionReady() const;
	virtual bool CanUseSolutionforQuery(const FPathFindingQuery& Query) const;
	FORCEINLINE bool GetFlowVectorForTile(const UGridTile * InTile, FVector& OutTile) const;

protected:
	virtual void LayerInitialize(AGameGrid* InGrid) override;
	virtual void OnLayerActivate(TArray<UGridTile*> TileSubset = TArray<UGridTile*>()) override;
	virtual void OnShowLayer() override;
	virtual void OnHideLayer() override;


protected:
	UPROPERTY(transient)
	UFlowFieldCostLayer* CostLayer = nullptr;

	UPROPERTY(transient)
	UFlowFieldIntegrationLayer* IntegrationLayer = nullptr;

	UPROPERTY(transient)
	UFlowFieldVectorLayer* VectorLayer = nullptr;

protected:
	bool bNeedsCostRebuild = true;

	bool bNeedsWeightRebuild = true;


};