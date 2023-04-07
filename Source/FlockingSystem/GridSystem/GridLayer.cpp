// Fill out your copyright notice in the Description page of Project Settings.


#include "GridLayer.h"
#include "GameGrid.h"

void UGridLayer::LayerInitialize(AGameGrid* InGrid, const TArray<UGridTile*>& InActiveTiles, AActor* InApplicator)
{
	ParentGrid = InGrid;
	ActiveTiles = TArray<UGridTile*>(InActiveTiles);
	Applicator = InApplicator;
}

void UGridLayer::OnLayerActivate()
{
	for (int i = 0; i < ActiveTiles.Num(); i++)
	{
		PostActivateTile(ActiveTiles[i]);
	}

}

uint32 UGridLayer::OnLayerDeactivate()
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

void UGridLayer::AddTile(UGridTile* InTile, const bool& InbShouldActivate)
{
	int32 isunique = TileSet.AddUnique(InTile);

	if ((isunique > INDEX_NONE) && InbShouldActivate)
	{
		ActiveTiles.Emplace(InTile);
		PostActivateTile(InTile);
	}
}

bool UGridLayer::RemoveTile(UGridTile* InTile)
{
	int32 removesuccess = ActiveTiles.Remove(InTile);

	if (removesuccess > (int32)0)
	{
		PostDeactivateTile(InTile);
	}

	removesuccess += TileSet.Remove(InTile);
	const bool retval = removesuccess > (int32)0;

	return retval;
}

TArray<UGridTile*> UGridLayer::GetDefaultTileSet(const AGameGrid* InGrid) const
{
	TArray<UGridTile*> retval = TArray<UGridTile*>();

	if (IsValid(InGrid))
	{
		retval = InGrid->GetTiles();
	}

	return retval;
}
