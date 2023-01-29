// Fill out your copyright notice in the Description page of Project Settings.


#include "GridTile.h"
#include "GameGrid.h"


AGameGrid* UGridTile::GetGameGrid() const
{
	return Cast<AGameGrid>(GetOuter());
}

int32 UGridTile::GetTileID() const
{
	return TileID;
}

bool UGridTile::IsTileValid() const
{
	return TileID != INVALID_TILE_ID;
}

FVector UGridTile::GetTileCenter() const
{
	return TileCenter;
}

TArray<UGridTile*> UGridTile::GetNeighbors() const
{
	return Neighbors;
}

void UGridTile::SetupTile(const int32 InID, const FVector InTileCenter)
{
	TileID = InID;
	TileCenter = InTileCenter;
}

void UGridTile::AddNeighbor(UGridTile* InNeighbor)
{
	Neighbors.AddUnique(InNeighbor);
}

TSet<FLine> UGridTile::GetTileBoundaryLines() const
{
	TSet<FLine> retval = TSet<FLine>();
	if (IsTileValid())
	{
		const AGameGrid* grid = GetGameGrid();
		const EGridTileType tileshape = grid->GetTileShape();
		const float edgelength = grid->GetTileEdgeLength();
		const FVector tilecenter = GetTileCenter();

		if (tileshape == EGridTileType::SQUARE)
		{
			float gridheight = tilecenter.Z;
			float offset = edgelength / 2.0f;
			FVector topleft = tilecenter + FVector(-offset, offset, gridheight);
			FVector topright = tilecenter + FVector(offset, offset, gridheight);
			FVector bottomleft = tilecenter + FVector(-offset, -offset, gridheight);
			FVector bottomright = tilecenter + FVector(offset, -offset, gridheight);

			retval.Add(FLine(topleft, topright));
			retval.Add(FLine(topright, bottomright));
			retval.Add(FLine(bottomright, bottomleft));
			retval.Add(FLine(bottomleft, topleft));
		}
		else if (EGridTileType::HEXAGON)
		{
			/*
			FVector top = tilecenter + FVector(0, tileradius, 0);
			FVector topleft = tilecenter + FVector(-tileradius * 0.5f, tileradius * 0.5f, 0);
			FVector topright = tilecenter + FVector(tileradius * 0.5f, tileradius * 0.5f, 0);
			FVector bottomleft = tilecenter + FVector(-tileradius * 0.5f, -tileradius * 0.5f, 0);
			FVector bottomright = tilecenter + FVector(tileradius * 0.5f, -tileradius * 0.5f, 0);
			FVector bottom = tilecenter + FVector(0, -tileradius, 0);

			retval.Add(FLine(top, topleft));
			retval.Add(FLine(topleft, bottomleft));
			retval.Add(FLine(bottomleft, bottom));
			retval.Add(FLine(bottom, bottomright));
			retval.Add(FLine(bottomright, topright));
			retval.Add(FLine(topright, top));
			*/
		}
	}

	return retval;
}

UWorld* UGridTile::GetWorld() const
{
	UWorld* retval = nullptr;
	AActor* actor = Cast<AActor>(GetOuter());

	if (IsValid(actor))
	{
		retval = actor->GetWorld();
	}

	return retval;
}

void UGridTile::DrawDebugData()
{
	DrawDebugString(GetWorld(), TileCenter, FString::FromInt(TileID), GetGameGrid(), FColor::Red);
}

uint32 GetTypeHash(const FLine& Thing)
{
	uint32 Hash = FCrc::MemCrc32(&Thing, sizeof(FLine));
	return Hash;
}
