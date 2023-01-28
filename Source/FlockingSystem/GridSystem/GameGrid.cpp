// Fill out your copyright notice in the Description page of Project Settings.


#include "GameGrid.h"

AGameGrid::AGameGrid() : Super()
{
    LinesProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Grid Line Drawing Component"));
}

int32 AGameGrid::GetMaxTiles() const
{
    int32 retval = 0U;
    if (TileShape == EGridTileType::SQUARE)
    {
        const int32 numRows = GetMaxRows();
        const int32 numCols = GetMaxCols();
        retval = numRows * numCols;
    }
    else if (TileShape == EGridTileType::HEXAGON)
    {
        const int32 numRows = GetMaxRows();
        const int32 numCols = GetMaxCols();
        retval = (numCols * (numRows + 1)) / 2;
    }

    return retval;
}

int32 AGameGrid::GetMaxRows() const
{
    int32 retval = 0;
    if (TileShape == EGridTileType::SQUARE)
    {
        retval = FMath::CeilToInt(GridLength / TileRadius);
    }
    else if (TileShape == EGridTileType::HEXAGON)
    {
        retval = FMath::CeilToInt((GridLength / (TileRadius * FMath::Sqrt(3.0f))) * 2.0f);
    }

    return retval;
}

int32 AGameGrid::GetMaxCols() const
{
    int32 retval = 0;
    if (TileShape == EGridTileType::SQUARE)
    {
        retval = FMath::CeilToInt(GridWidth / TileRadius);
    }
    else if (TileShape == EGridTileType::HEXAGON)
    {
        retval = FMath::CeilToInt(GridWidth / (TileRadius * 1.5f));
    }

    return retval;
}

EGridTileType AGameGrid::GetTileShape() const
{
    return TileShape;
}

float AGameGrid::GetTileRadius() const
{
    return TileRadius;
}

bool AGameGrid::IsValidGridLocation(const FVector& InLocation) const
{
    // Check if the location is within the grid's bounding box
    return InLocation.X >= GetActorLocation().X && InLocation.X <= GetActorLocation().X + GridWidth
        && InLocation.Y >= GetActorLocation().Y && InLocation.Y <= GetActorLocation().Y + GridLength;
}

void AGameGrid::DrawTiles(const TSet<FLine>& InGridLines)
{
    UMaterialInstanceDynamic* linematerial = UMaterialInstanceDynamic::Create(GridMaterial, this);

    linematerial->SetVectorParameterValue(ColorParameterName, LineColor);
    linematerial->SetScalarParameterValue(OpacityParameterName, LineOpacity);
    
    TArray<FVector> verticies = TArray<FVector>();
    TArray<int> tris = TArray<int>();
    int32 sectionid = 0;

    for (const FLine& Elem : InGridLines)
    {
        verticies.Reset();
        tris.Reset();
        BuildLineRenderData(Elem.StartPoint, Elem.EndPoint, GridLineThickness, verticies, tris);

        LinesProceduralMesh->CreateMeshSection(sectionid, verticies, tris, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), false);
        LinesProceduralMesh->SetMaterial(sectionid, linematerial);
        sectionid++;
    }

}

bool AGameGrid::BuildGridData(TSet<FLine>& OutGridLines)
{
	bool retval = true;
    
    // Clear previous data todo: actually delete the tiles / do better cleanup
    GridData.Empty();

    // Create new grid data
    const int32 numtiles = GetMaxTiles();
    for (int32 i = 0; i < numtiles; i++)
    {
        FVector tilecenterlocation = FVector();

        if (DetermineTileLocation(i, tilecenterlocation))
        {
            UGridTile* newtile = NewObject<UGridTile>(this, TileClass);
            newtile->SetupTile(i, tilecenterlocation);
            
            const TSet<FLine> boundarylines = newtile->GetTileBoundaryLines();
            OutGridLines.Append(boundarylines);

            GridData.Emplace(newtile);
        }
    }

    AddTileNeighbors();

	return retval;
}

void AGameGrid::AddTileNeighbors()
{
    const int32 numrows = GetMaxRows();
    const int32 numcols = GetMaxCols();

    for (int32 i = 0; i < GridData.Num(); i++)
    {
        const int32 col = i % numcols;
        const int32 row = i / numcols;
        UGridTile* currenttile = GridData[i];

        if (TileShape == EGridTileType::SQUARE)
        {
            if (row > 0)
            {
                currenttile->AddNeighbor(GridData[i - numcols]);
            }
            // Check bottom neighbor
            if (row < numrows - 1)
            {
                currenttile->AddNeighbor(GridData[i + numcols]);
            }
            // Check left neighbor
            if (col > 0)
            {
                currenttile->AddNeighbor(GridData[i - 1]);
            }
            // Check right neighbor
            if (col < numcols - 1)
            {
                currenttile->AddNeighbor(GridData[i + 1]);
            }
        }
        else if (TileShape == EGridTileType::HEXAGON)
        {
            // Check top-left neighbor
            if (row > 0 && col > 0)
            {
                currenttile->AddNeighbor(GridData[i - numcols - 1]);
            }
            // Check top-right neighbor
            if (row > 0 && col < numcols - 1)
            {
                currenttile->AddNeighbor(GridData[i - numcols + 1]);
            }
            // Check bottom-left neighbor
            if (row < numrows - 1 && col > 0)
            {
                currenttile->AddNeighbor(GridData[i + numcols - 1]);
            }
            // Check bottom-right neighbor
            if (row < numrows - 1 && col < numcols - 1)
            {
                currenttile->AddNeighbor(GridData[i + numcols + 1]);
            }
            // Check left neighbor
            if (col > 0)
            {
                currenttile->AddNeighbor(GridData[i - 1]);
            }
            // Check right neighbor
            if (col < numcols - 1)
            {
                currenttile->AddNeighbor(GridData[i + 1]);
            }
        }
    }
}

void AGameGrid::PostInitializeComponents()
{
	Super::PostInitializeComponents();

    TSet<FLine> lineset = TSet<FLine>();
    if (BuildGridData(lineset)  && IsValid(LinesProceduralMesh))
    {
        DrawTiles(lineset);
    }
}

int32 AGameGrid::GetNumTiles()
{
    return GridData.Num();
}

bool AGameGrid::DetermineTileLocation(int32 InTileID, FVector& OutTileCenter)
{
    bool retval = true;

    const FVector actorlocation = GetActorLocation();

    // Calculate the number of rows and columns in the grid
    const int32 numRows = FMath::CeilToInt(GridLength / TileRadius);
    const int32 numCols = FMath::CeilToInt(GridWidth / TileRadius);

    // Calculate the x and y coordinates of the tile
    const int32 x = InTileID % numCols;
    const int32 y = InTileID / numCols;

    // Determine the shape of the tile
    if (TileShape == EGridTileType::SQUARE)
    {
        OutTileCenter = actorlocation + FVector(x * TileRadius, y * TileRadius, 0);
    }
    else if (TileShape == EGridTileType::HEXAGON)
    {
        // Calculate the distance between the center of each hexagon in the x and y direction
        const float xSpacing = (3.0f / 2.0f) * TileRadius;
        const float ySpacing = TileRadius * FMath::Sqrt(3.0f);

        OutTileCenter = actorlocation + FVector(x * xSpacing, y * ySpacing, 0);
    }

    // Check if the tile is within the bounds of the grid
    if (OutTileCenter.X < 0 || OutTileCenter.X > GridWidth || OutTileCenter.Y < 0 || OutTileCenter.Y > GridLength)
    {
        retval = false;
    }

    return retval;
}

void AGameGrid::BuildLineRenderData(const FVector LineStart, const FVector LineEnd, const float LineThickness, TArray<FVector>& Verts, TArray<int>& Tris)
{
	const float halfthickness = LineThickness / 2.0f;

	/*A Line Segment is made of two triangles forming a rectangle, get the hypotenuse, then cross with z to get the grid direction*/
	const FVector trianglehypotenuse = (LineEnd - LineStart).GetSafeNormal();
	const FVector griddirection = FVector::CrossProduct(trianglehypotenuse, FVector(0.0f, 0.0f, 1.0f));
	const FVector vertexadjustment = griddirection * halfthickness;

	/*Create and Append the new triangle vertex map so we know which verticies make up the two triangles*/
	int* newtriangles = new int[6]();

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

	FVector* newverticies = new FVector[4]();
	newverticies[0] = LineStart + vertexadjustment;
	newverticies[1] = LineEnd + vertexadjustment;
	newverticies[2] = LineStart - vertexadjustment;
	newverticies[3] = LineEnd - vertexadjustment;

	Verts.Append(newverticies, 4);
	delete[] newverticies;
}
