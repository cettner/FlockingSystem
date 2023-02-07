// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../GridLayer.h"
#include "FlowFieldCostLayer.generated.h"

constexpr uint8 BLOCKED_TILE_COST = 0xFF;

UCLASS()
class FLOCKINGSYSTEM_API UFlowFieldCostLayer : public UGridLayer
{
	GENERATED_BODY()
	
protected:
	virtual void LayerInitialize(AGameGrid* InGrid);
	virtual uint8 CalculateTileCost(const UGridTile * InTile);

protected:
	UPROPERTY(EditDefaultsOnly, Category = Setup)
	TEnumAsByte<ECollisionChannel> BlockTraceChannel = ECollisionChannel::ECC_WorldStatic;

	UPROPERTY(EditDefaultsOnly, Category = Setup)
	float BlockTraceHeightOffset = 200.0f;

	UPROPERTY(EditDefaultsOnly, Category = Setup)
	float BlockTraceRadius = 30.0f;

protected:
	TMap<UGridTile *, uint8> CostMap = TMap<UGridTile *, uint8>();
};
