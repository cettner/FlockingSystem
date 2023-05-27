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

public:
	void AddGoalTile(UGridTile* InTile, const bool bRebuildWeights = false);
	void AddGoalTile(TArray<UGridTile*> InTiles, const bool bRebuildWeights = false);
	virtual void SubscribeAgent(const UObject * Subscriber);
	virtual void UnSubscribeAgent(const UObject* UnSubScriber);
	FORCEINLINE virtual bool IsAgentSubscribed(const UObject* UnSubScriber) const;

	bool HasGoal() const;
	FORCEINLINE bool IsGoalTile(const UGridTile * InGridTile) const;
	virtual bool RequiresCostRebuild() const;
	virtual bool RequiresWeightRebuild() const;
	virtual bool IsSolutionReady() const;
	virtual bool CanUseSolutionforQuery(const FPathFindingQuery& Query) const;
	FORCEINLINE bool GetFlowVectorForTile(const UGridTile * InTile, FVector& OutTile) const;

protected:
	virtual bool BuildSolution();
	virtual void LayerInitialize(AGameGrid* InGrid, const TArray<UGridTile*>& InActiveTiles, AActor* InApplicator) override;
	virtual void OnCostLayerRebuilt(UFlowFieldCostLayer * InRebuiltLayer);
	virtual bool InitializeCostData();
	virtual void OnLayerActivate() override;
	virtual void OnShowLayer() override;
	virtual void OnHideLayer() override;


protected:
	/*Since Cost Data Exists in SuperTiles, we reassemble the cost data here from each of them*/
	UPROPERTY(transient)
	TMap<UGridTile*, uint8> CostMap = TMap<UGridTile*, uint8>();

	UPROPERTY(transient)
	UFlowFieldIntegrationLayer* IntegrationLayer = nullptr;

	UPROPERTY(transient)
	UFlowFieldVectorLayer* VectorLayer = nullptr;

	TSet<const UObject*> Subscribers = TSet<const UObject*>();

protected:
	bool bNeedsCostRebuild = true;

	bool bNeedsWeightRebuild = true;


};