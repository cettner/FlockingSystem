// Fill out your copyright notice in the Description page of Project Settings.


#include "GridLayer.h"
#include "GameGrid.h"

void UGridLayer::LayerInitialize(AGameGrid* InGrid)
{
	ParentGrid = InGrid;
}

void UGridLayer::OnLayerActivate(TArray<UGridTile*> TileSubset)
{
	const TArray<UGridTile*>& gridtiles = TileSubset.Num() > 0 ? TileSubset : ParentGrid->GetTiles();

	for (int i = 0; i < gridtiles.Num(); i++)
	{
		ActiveTiles.Emplace(gridtiles[i]);
		PostActivateTile(gridtiles[i]);
	}

}

uint32 UGridLayer::OnLayerDeactivate(TArray<UGridTile*> TileSubset)
{
	return false;
}

void UGridLayer::OnShowLayer()
{
	for (int i = 0; i < ActiveTiles.Num(); i++)
	{
		ShowTile(ActiveTiles[i]);
	}
}

void UGridLayer::OnHideLayer()
{
	for (int i = 0; i < ActiveTiles.Num(); i++)
	{
		HideTile(ActiveTiles[i]);
	}
}

void UGridLayer::PostActivateTile(UGridTile* InTile)
{
}

void UGridLayer::PostDeactivateTile(UGridTile* InTile)
{
}

void UGridLayer::ShowTile(UGridTile* InTile)
{
}

void UGridLayer::HideTile(UGridTile* InTile)
{
}

void UGridLayer::SetLayerVisibility(bool InbIsVisible)
{
	bIsLayerVisible = InbIsVisible;

	if (bIsLayerVisible == true)
	{
		OnShowLayer();
	}
	else
	{
		OnHideLayer();
	}
}
