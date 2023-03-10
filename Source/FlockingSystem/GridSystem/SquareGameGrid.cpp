// Fill out your copyright notice in the Description page of Project Settings.


#include "SquareGameGrid.h"
#include "Materials/MaterialInstance.h"

#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
ASquareGameGrid::ASquareGameGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	LinesProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("GridMesh"));
	SelectionProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("SelectionMesh"));

	LinesProceduralMesh->SetupAttachment(RootComponent);
	LinesProceduralMesh->SetVisibility(true, true);
	LinesProceduralMesh->SetHiddenInGame(true, true);

	SelectionProceduralMesh->SetupAttachment(RootComponent);
	SelectionProceduralMesh->SetVisibility(true, true);
	SelectionProceduralMesh->SetHiddenInGame(true, true);
}

void ASquareGameGrid::PostInitializeComponents()
{
	Super::PostInitializeComponents();


	
	BuildGridData();
	DrawGrid();
	DrawTiles();

}

void ASquareGameGrid::SetSelectedTile(TArray<FGridTileData> SelectedTiles)
{
		for (int i = 0; i < SelectedTiles.Num(); i++)
		{
			SetSelectedTile(SelectedTiles[i]);
		}
}

void ASquareGameGrid::SetSelectedTile(FGridTileData SelectedTile)
{
	int32 gridid = GetUniqueGridID(SelectedTile);
	if ((gridid != INVALID_TILE_ID))
	{
		SelectionProceduralMesh->SetMeshSectionVisible(gridid, true);
	}
}

void ASquareGameGrid::HideSelectedTile(TArray<FGridTileData> SelectedTiles)
{
	for (int i = 0; i < SelectedTiles.Num(); i++)
	{
		HideSelectedTile(SelectedTiles[i]);
	}
}

void ASquareGameGrid::HideSelectedTile(FGridTileData SelectedTile)
{
	int32 gridid = GetUniqueGridID(SelectedTile);
	if ((gridid != INVALID_TILE_ID))
	{
		SelectionProceduralMesh->SetMeshSectionVisible(gridid, false);
	}
}

void ASquareGameGrid::HideGrid()
{
	LinesProceduralMesh->SetHiddenInGame(true, true);
	SelectionProceduralMesh->SetHiddenInGame(true, true);
}

void ASquareGameGrid::ShowGrid()
{
	LinesProceduralMesh->SetHiddenInGame(false, true);
	SelectionProceduralMesh->SetHiddenInGame(false, true);
}

float ASquareGameGrid::GetGridWidth() const
{
	return NumColumns * TileSize;
}

float ASquareGameGrid::GetGridHieght() const
{
	return NumRows * TileSize;
}

float ASquareGameGrid::GetTileSize() const
{
	return TileSize;
}

float ASquareGameGrid::GetGridElevation(FGridTileData Tiledata) const
{
	return GetActorLocation().Z;
}

bool ASquareGameGrid::IsTileValid(int Row, int Column) const
{
	if (Row >= 0 && Row < NumRows && Column >= 0 && Column < NumColumns)
	{
		return(true);
	}

	return(false);
}

FGridTileData ASquareGameGrid::GetTileFromLocation(FVector Location) const
{
	FGridTileData retval;
	FVector gridstartlocation = GetActorLocation();
	float approxcol = ((Location.Y - gridstartlocation.Y) / GetGridHieght()) * NumColumns;
	float approxrow = ((Location.X - gridstartlocation.X) / GetGridWidth()) * NumRows;
	float centeroffset = TileSize / 2.0f;

	retval.row = floorf(approxrow);
	retval.column = floorf(approxcol);
	retval.IsValid = IsTileValid(retval.row, retval.column);

	if (retval.IsValid)
	{
		retval.TileCenter.X = gridstartlocation.X + (retval.row * TileSize) + centeroffset;
		retval.TileCenter.Y = gridstartlocation.Y + (retval.column * TileSize) + centeroffset;
		retval.TileCenter.Z = GetGridElevation(retval);
	}


	return (retval);
}

bool ASquareGameGrid::GetLocationFromTile(FGridTileData Tiledata ,FVector & OutLocation, bool Center) const
{

	if (!IsTileValid(Tiledata.row, Tiledata.column))
	{
		OutLocation = FVector(0.0f, 0.0f, 0.0f);
		return(false);
	}
	
	OutLocation.Z = GetGridElevation(Tiledata);

	FVector gridstartlocation = GetActorLocation();

	if (Center)
	{
		float centeroffset = TileSize / 2.0f;
		OutLocation.X = gridstartlocation.X + (Tiledata.row * TileSize) + centeroffset;
		OutLocation.Y = gridstartlocation.Y + (Tiledata.column * TileSize) + centeroffset;
	}
	else
	{
		OutLocation.X = gridstartlocation.X + (Tiledata.row * TileSize);
		OutLocation.Y = gridstartlocation.Y + (Tiledata.column * TileSize);
	}

	return(true);
}

int32 ASquareGameGrid::GetUniqueGridID(FGridTileData Tiledata) const
{
	int32 retval = INVALID_TILE_ID;
	if (IsTileValid(Tiledata.row,Tiledata.column))
	{
		retval = (Tiledata.row * NumColumns) + Tiledata.column;
	}

	return retval;
}

void ASquareGameGrid::DrawLine(FVector LineStart, FVector LineEnd, float LineThickness, TArray<FVector>& Verts, TArray<int>& Tris)
{
	const float halfthickness = LineThickness / 2.0f;

	/*A Line Segment is made of two triangles forming a rectangle, get the hypotenuse, then cross with z to get the grid direction*/
	const FVector trianglehypotenuse= (LineEnd - LineStart).GetSafeNormal();
	const FVector griddirection =   FVector::CrossProduct(trianglehypotenuse, FVector(0.0f, 0.0f, 1.0f));
	const FVector vertexadjustment = griddirection * halfthickness;

	/*Create and Append the new triangle vertex map so we know which verticies make up the two triangles*/
	int * newtriangles = new int[6]();

	const int numverts = Verts.Num();
	newtriangles[0] = numverts + 2;
	newtriangles[1] = numverts + 1;
	newtriangles[2] = numverts;
	newtriangles[3] = numverts + 2;
	newtriangles[4] = numverts + 3;
	newtriangles[5] = numverts + 1;

	Tris.Append(newtriangles, 6);
	delete[] newtriangles;
	

	/*Create and Append the new Verticies*/

	FVector * newverticies = new FVector[4]();
	newverticies[0] = LineStart + vertexadjustment;
	newverticies[1] = LineEnd + vertexadjustment;
	newverticies[2] = LineStart - vertexadjustment;
	newverticies[3] = LineEnd - vertexadjustment;

	Verts.Append(newverticies, 4);
	delete[] newverticies;
}

void ASquareGameGrid::DrawGrid()
{
	if (GridMaterial)
	{
		LineTriangles.Empty();
		LineVerticies.Empty();

		UMaterialInstanceDynamic * linematerial = UMaterialInstanceDynamic::Create(GridMaterial, this);

		linematerial->SetVectorParameterValue(ColorParameterName, LineColor);
		linematerial->SetScalarParameterValue(OpacityParameterName, LineOpacity);


		/*Obtain Horizontal Line Data*/
		for (int i = 0; i <= NumRows; i++)
		{
			float linestartpos = i * TileSize;
			FVector LineStart = FVector(linestartpos, 0.0f, 0.0f);
			FVector LineEnd = FVector(linestartpos, GetGridWidth(), 0.0f);

			DrawLine(LineStart, LineEnd, GridLineThickness, LineVerticies, LineTriangles);
		}

		/*Obtain Verticle Lines Data*/
		for (int i = 0; i <= NumColumns; i++)
		{
			float linestartpos = i * TileSize;
			FVector LineStart = FVector(0.0f, linestartpos, 0.0f);
			FVector LineEnd = FVector(GetGridHieght(), linestartpos, 0.0f);

			DrawLine(LineStart, LineEnd, GridLineThickness, LineVerticies, LineTriangles);
		}


		if (LinesProceduralMesh)
		{
			LinesProceduralMesh->CreateMeshSection(0, LineVerticies, LineTriangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), false);
			LinesProceduralMesh->SetMaterial(0, linematerial);
		}
	}

}

void ASquareGameGrid::DrawTiles()
{
	for (int32 i = 0; i < GridData.Num(); i++)
	{
		TArray<FVector> selectedverts = TArray<FVector>();
		TArray<int> selectedtris = TArray<int>();
		FGridTileData currenttile = GridData[i];

		if (IsTileValid(currenttile.row, currenttile.column))
		{
			float ystart = (TileSize / 2.0f) + (currenttile.column * TileSize);
			float xstart = currenttile.row * TileSize;
			float xend = TileSize + (TileSize * currenttile.row);
			float yend = (TileSize / 2.0f) + (currenttile.column * TileSize);

			FVector selectstart = FVector(xstart, ystart, 0.0f);
			FVector selectend = FVector(xend, yend, 0.0f);
			
			/*Draw a single fat "Line" with the width the size of the Tile*/
			DrawLine(selectstart, selectend, TileSize, selectedverts, selectedtris);
		}

		if (SelectionProceduralMesh && GridMaterial)
		{
			UMaterialInstanceDynamic * selectionmaterial = UMaterialInstanceDynamic::Create(GridMaterial, this);
			selectionmaterial->SetVectorParameterValue(ColorParameterName, DefaultSelectionColor);
			selectionmaterial->SetScalarParameterValue(OpacityParameterName, DefaultSelectionOpacity);

			SelectionProceduralMesh->CreateMeshSection(i, selectedverts, selectedtris, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), false);
			SelectionProceduralMesh->SetMaterial(i, selectionmaterial);

			SelectionProceduralMesh->SetMeshSectionVisible(i, false);
		}
	}
}

bool ASquareGameGrid::BuildGridData()
{
	GridData.Empty();
	bool buildsuccess = true;

	for (int i = 0; i < NumRows; i++)
	{
		for (int k = 0; k < NumColumns; k++)
		{
			FGridTileData Currentile;
			Currentile.row = i;
			Currentile.column = k;
			Currentile.IsValid = GetLocationFromTile(Currentile, Currentile.TileCenter);
			GridData.Emplace(Currentile);

			buildsuccess &= Currentile.IsValid;
		}
	}

	return(buildsuccess);
}


#ifdef WITH_EDITOR
void ASquareGameGrid::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);
	UWorld * World = GetWorld();

	if (World)
	{
		EWorldType::Type type = World->WorldType;
		if (type >= EWorldType::Editor)
		{
			BuildGridData();
			DrawGrid();
			DrawTiles();
		}
	}
}
#endif