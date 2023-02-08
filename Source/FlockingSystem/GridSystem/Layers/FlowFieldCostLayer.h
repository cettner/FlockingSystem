// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../GridLayer.h"
#include "FlowFieldCostLayer.generated.h"

constexpr uint8 BLOCKED_TILE_COST = 0xFFU;
constexpr uint8 FREE_TILE_COST = 0x00U;

UCLASS()
class FLOCKINGSYSTEM_API UFlowFieldCostLayer : public UGridLayer
{
	GENERATED_BODY()
	

public:
	virtual bool GetTileCost(UGridTile* InTile, uint8& OutCost);

protected:
	virtual void LayerInitialize(AGameGrid* InGrid) override;
	virtual void PostActivateTile(UGridTile* InTile) override;

protected:
	virtual void CalculateTiles(const TArray<UGridTile*>& InTiles);
	virtual uint8 CalculateTileCost(const UGridTile * InTile);
	virtual FLinearColor GetCostFillColor(const uint8 InCost);

protected:
	UPROPERTY(EditDefaultsOnly, Category = Setup)
	TEnumAsByte<ECollisionChannel> BlockTraceChannel = ECollisionChannel::ECC_WorldStatic;

	UPROPERTY(EditDefaultsOnly, Category = Setup)
	float BlockTraceHeightOffset = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = Setup)
	float BlockTraceRadius = 30.0f;

protected:
	TMap<UGridTile *, uint8> CostMap = TMap<UGridTile *, uint8>();
};
