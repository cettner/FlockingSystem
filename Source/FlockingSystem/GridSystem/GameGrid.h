// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridTile.h"
#include "GridLayer.h"
#include "ProceduralMeshComponent.h"

#include "GameGrid.generated.h"





UCLASS()
class FLOCKINGSYSTEM_API AGameGrid : public AActor
{
	GENERATED_BODY()

protected:
	AGameGrid();

public:
	virtual int32 GetMaxTiles() const;
	virtual int32 GetMaxRows() const;
	virtual int32 GetMaxCols() const;
	const TArray<UGridTile*>& GetTiles() const { return GridData; }
	virtual EGridTileType GetTileShape() const;
	virtual float GetTileRadius() const;
	virtual float GetTileEdgeLength() const;
	virtual bool IsValidGridLocation(const FVector& InLocation) const;
	virtual void SetTileVisible(int32 TileID, bool bIsVisble) const;
	virtual void SetTileColor(int32 TileID, FLinearColor InTileColor);
	
	template<typename T>
	T* GetLayerOfClass() const
	{
		T* retval = nullptr;
		for (int32 i = 0; i < GridLayers.Num(); i++)
		{
			if (T* layer = Cast<T>(this->GridLayers[i]))
			{
				retval = layer;
				break;
			}
		}

		return retval;
	}

public:
	virtual UGridLayer* AddGridLayer(TSubclassOf<UGridLayer> InLayerClass);
	virtual void SetActiveLayer(UGridLayer* InLayer, TArray<UGridTile*> InTileSubset = TArray<UGridTile*>());

protected:
	virtual void DrawGridLines(const TSet<FLine>& InGridLines);

	virtual void DrawGridTiles();

	virtual bool BuildGridData(TSet<FLine>& OutGridLines);

	virtual void AddTileNeighbors();

	virtual void InitializeLayers();

	virtual int32 GetNumTiles();

	virtual bool DetermineTileLocation(const int32 InRow, const int32 InCol, FVector& OutTileCenter);

protected:
	virtual void BuildLineRenderData(const FVector LineStart, const FVector LineEnd, const float LineThickness, TArray<FVector>& Verts, TArray<int>& Tris);

protected:
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

protected:
	virtual void DrawDebugData();


protected:
	/*Width of Drawn Line*/
	UPROPERTY(EditAnywhere, Category = "Setup")
	float GridLineThickness = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Setup")
	TEnumAsByte<EGridTileType> TileShape = EGridTileType::SQUARE;

	/*Tile Radius*/
	UPROPERTY(EditAnywhere, Category = "Setup")
	float TileEdgeLength = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Setup")
	float GridWidth = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Setup")
	float GridLength = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSubclassOf<UGridTile> TileClass = UGridTile::StaticClass();

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	UMaterial* GridMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	FName ColorParameterName = "Color";

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	FLinearColor LineColor;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	FName OpacityParameterName = "Opacity";

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float LineOpacity = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	FLinearColor DefaultTileFillColor = FLinearColor::Red;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float DefaultTileFillOpacity = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TArray<TSubclassOf<UGridLayer>> StartupLayers = TArray<TSubclassOf<UGridLayer>>();

protected:
	UPROPERTY(Transient)
	TArray<UGridTile*> GridData = TArray<UGridTile*>();

	UPROPERTY(Transient)
	TArray<UGridLayer*> GridLayers = TArray<UGridLayer*>();

	bool bIsGridDataBuilt = false;

	UProceduralMeshComponent* LinesProceduralMesh = nullptr;

	UProceduralMeshComponent* SelectionProceduralMesh = nullptr;

#ifdef WITH_EDITOR

protected:
	virtual void RebuildGridData(bool bRedrawMesh = true);

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	bool bRebuildGridData = true;
#endif

};
