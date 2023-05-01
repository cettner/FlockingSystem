// Fill out your copyright notice in the Description page of Project Settings.


#include "SuperTileManager.h"
#include "..\GameGrid.h"

USuperTileManager::USuperTileManager()
{
    bIsLayerVisible = true;
}

void USuperTileManager::InitializeTileNeighbors(TArray<UGridLayer*>& InLayers)
{
    // Loop through all the super tiles and assign their neighbors
    for (int32 i = 0; i < InLayers.Num(); i++)
    {
        FSuperTile supertile = FSuperTile();
        supertile.AppliedLayer = InLayers[i];

        // Get the row and column indices of the super tile
        const int32 rowindex = i / RowSize;
        const int32 colindex = i % ColomnSize;

        // Loop through all adjacent super tiles
        for (int32 AdjRowIndex = rowindex - 1; AdjRowIndex <= rowindex + 1; AdjRowIndex++)
        {
            for (int32 AdjColIndex = colindex - 1; AdjColIndex <= colindex + 1; AdjColIndex++)
            {
                // Skip if the adjacent tile is out of bounds or if it's the current tile
                if (AdjRowIndex < 0 || AdjRowIndex >= RowSize || AdjColIndex < 0 || AdjColIndex >= ColomnSize || (AdjRowIndex == rowindex && AdjColIndex == colindex))
                {
                    continue;
                }

                // Get the index of the adjacent super tile
                const int32 AdjIndex = AdjRowIndex * ColomnSize + AdjColIndex;

                // Add the adjacent tile as a neighbor
                UGridLayer* AdjSuperTile = InLayers[AdjIndex];
                supertile.TileNeighbors.Add(AdjSuperTile);
            }
        }
        SuperTiles.Add(supertile);
    }
}

FLinearColor USuperTileManager::GetTileHashColor(const int& InHashID)
{
    const int32 Hue = InHashID % 360;
    const int32 Saturation = 150 + (InHashID * 31) % 101; // Keep it in the range of 150-250
    const int32 Value = 150 + ((InHashID * 17) % 51); // Keep it in the range of 150-200
    const FLinearColor retval = FLinearColor::MakeFromHSV8(Hue, Saturation, Value);

    return retval;
}

void USuperTileManager::LayerInitialize(AGameGrid* InGrid, const TArray<UGridTile*>& InActiveTiles, AActor* InApplicator)
{
    Super::LayerInitialize(InGrid, InActiveTiles, InApplicator);

    if (LayerClass != nullptr)
    {
        TArray<UGridLayer*> superlayers = TArray<UGridLayer*>();

        // Create the super tiles
        const int32 numrows = InGrid->GetMaxRows() / RowSize;
        const int32 numcols = InGrid->GetMaxCols() / ColomnSize;

        for (int32 rowindex = 0; rowindex < numrows; rowindex++)
        {
            for (int32 colindex = 0; colindex < numcols; colindex++)
            {
                TSet<UGridTile*> subsettiles;
                for (int32 subrowindex = 0; subrowindex < RowSize; subrowindex++)
                {
                    for (int32 subcolindex = 0; subcolindex < ColomnSize; subcolindex++)
                    {
                        const int32 globalrowindex = rowindex * RowSize + subrowindex;
                        const int32 globalcolindex = colindex * ColomnSize + subcolindex;
                        const int32 tileindex = globalrowindex * InGrid->GetMaxCols() + globalcolindex;
                        if (tileindex < InActiveTiles.Num())
                        {
                            subsettiles.Add(InActiveTiles[tileindex]);
                        }
                    }
                }

                UGridLayer* newsupertile = InGrid->AddGridLayer(LayerClass, subsettiles.Array(), InApplicator);
                checkf(newsupertile, TEXT("USuperTileManager::LayerInitialize Failed to create supertile"));
                superlayers.Emplace(newsupertile);
            }
        }

        InitializeTileNeighbors(superlayers);
    }
}

void USuperTileManager::OnShowLayer()
{
    for (int32 i = 0; i < SuperTiles.Num(); i++)
    {
       const TSet<UGridTile*> tiles = SuperTiles[i]->GetTiles();
       const int32 layerid = SuperTiles[i]->GetLayerID();
       const FLinearColor supertilecolor = GetTileHashColor(layerid);
       for (UGridTile * tile : tiles)
       {
           tile->SetTileFillColor(supertilecolor);
           tile->SetTileVisible(true);
       }
    }
}
