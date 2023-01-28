// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "SquareGameGrid.generated.h"


constexpr int32 INVALID_TILE_ID = 0xFFFFFFFF;

USTRUCT()
struct FGridTileData
{
	GENERATED_USTRUCT_BODY()

	int row = -1;
	int column = -1;
	FVector TileCenter = FVector();
	bool IsValid = false;

	friend bool operator == (const FGridTileData& Myself, const FGridTileData& Other) 
	{ 
		bool isSame = Myself.row == Other.row;
		isSame &= Myself.column == Other.column;

		return(isSame); 
	}
};


UCLASS()
class ASquareGameGrid : public AActor
{
	GENERATED_BODY()
	
public:	
	ASquareGameGrid();

protected:
	virtual void PostInitializeComponents() override;



public:
	
	void SetSelectedTile(TArray<FGridTileData> SelectedTiles);
	void SetSelectedTile(FGridTileData SelectedTile);

	void HideSelectedTile(TArray<FGridTileData> SelectedTiles);
	void HideSelectedTile(FGridTileData SelectedTile);

	void HideGrid();
	void ShowGrid();

	float GetGridWidth() const;
	float GetGridHieght() const;
	float GetTileSize() const;

	virtual float GetGridElevation(FGridTileData Tiledata) const;

	bool IsTileValid(int Row, int Column) const;
	FGridTileData GetTileFromLocation(FVector Location) const;
	bool GetLocationFromTile(FGridTileData Tiledata, FVector & OutLocation, bool Center = true) const;
	int32 GetUniqueGridID(FGridTileData Tiledata) const;

protected:
	virtual void DrawLine(FVector LineStart, FVector LineEnd, float LineThickness, TArray<FVector>& Verts, TArray<int>& Tris);
	virtual void DrawGrid();
	virtual void DrawTiles();
	virtual bool BuildGridData();

protected:
	UPROPERTY(EditAnywhere, Category = "Setup")
	int NumRows = 10;

	UPROPERTY(EditAnywhere, Category = "Setup")
	int NumColumns = 10;

	UPROPERTY(EditAnywhere, Category = "Setup")
	float TileSize = 200.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float GridLineThickness = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	FLinearColor LineColor;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	FLinearColor DefaultSelectionColor;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float LineOpacity = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	float DefaultSelectionOpacity = 0.5f;


protected:
	UProceduralMeshComponent * LinesProceduralMesh;

	UProceduralMeshComponent * SelectionProceduralMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Procedural Mesh")
	UMaterial * GridMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Procedural Mesh")
	FName ColorParameterName = "Color";

	UPROPERTY(EditDefaultsOnly, Category = "Procedural Mesh")
	FName OpacityParameterName = "Opacity";

	UPROPERTY(BlueprintReadOnly)
	TArray<int> LineTriangles = TArray<int>();

	UPROPERTY(BlueprintReadOnly)
	TArray<FVector> LineVerticies = TArray<FVector>();

	TArray<FGridTileData> GridData = TArray<FGridTileData>();



#ifdef WITH_EDITOR
protected:
	virtual void OnConstruction(const FTransform & Transform) override;
#endif

};
