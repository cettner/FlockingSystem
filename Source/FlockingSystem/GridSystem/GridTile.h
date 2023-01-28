// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GridTile.generated.h"



constexpr int32 INVALID_TILE_ID = 0xFFFFFFFF;

UENUM()
enum EGridTileType
{
    TRIANGLE,
    SQUARE,
    HEXAGON
};

USTRUCT()
struct FLine
{
    GENERATED_USTRUCT_BODY()
public:
    FLine()
    {
        StartPoint = FVector();
        EndPoint = FVector();
    }

    FLine(const FVector &InStart, const FVector &InEnd) 
    {
        StartPoint = InStart;
        EndPoint = InEnd;
    }

    friend bool operator == (const FLine& Myself, const FLine& Other)
    {
        const bool bstartpointsame = (Myself.StartPoint == Other.StartPoint);
        const bool bendpointsame = (Myself.EndPoint == Other.EndPoint);
        return bstartpointsame && bendpointsame;
    }

    FVector StartPoint;
    FVector EndPoint;


};

uint32 GetTypeHash(const FLine& Thing);


UCLASS()
class FLOCKINGSYSTEM_API UGridTile : public UObject
{
    friend class AGameGrid;

	GENERATED_BODY()

public:
    int32 GetTileID() const;
    AGameGrid* GetGameGrid() const;
    template<class T>
    T* GetGameGrid() const { Cast<T>(GetGameGrid()); }
    bool IsTileValid() const;
    FORCEINLINE FVector GetTileCenter() const;

protected: 
    void SetupTile(const int32 InID, const FVector InTileCenter);
    void AddNeighbor(UGridTile* InNeighbor);
    TSet<FLine> GetTileBoundaryLines() const;

protected:
    int32 TileID = INVALID_TILE_ID;

    FVector TileCenter = FVector();

    UPROPERTY();
    TArray<UGridTile*> Neighbors = TArray<UGridTile*>();

    TArray<FVector> TileBounds = TArray<FVector>();
};
