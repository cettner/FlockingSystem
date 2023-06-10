// Fill out your copyright notice in the Description page of Project Settings.


#include "GameGrid.h"
#include "NavigationSystem.h"
#include "EngineUtils.h"

#include "../Navigation/VectorFieldNavigationSystem.h"
#include "GridAttachmentActor.h"

AGameGrid::AGameGrid() : Super()
{
    LinesProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Grid Line Drawing Component"));
    SelectionProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Tile Drawing Component"));

    FindVectorFieldImplementation = FindVectorField;
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
        retval = FMath::FloorToInt(GridLength / TileEdgeLength);
    }
    else if (TileShape == EGridTileType::HEXAGON)
    {
        retval = FMath::FloorToInt((GridLength / (TileEdgeLength * FMath::Sqrt(3.0f))) * 2.0f);
    }

    return retval;
}

int32 AGameGrid::GetMaxCols() const
{
    int32 retval = 0;
    if (TileShape == EGridTileType::SQUARE)
    {
        retval = FMath::FloorToInt(GridWidth / TileEdgeLength);
    }
    else if (TileShape == EGridTileType::HEXAGON)
    {
        retval = FMath::FloorToInt(GridWidth / (TileEdgeLength * 1.5f));
    }

    return retval;
}  

EGridTileType AGameGrid::GetTileShape() const
{
    return TileShape;
}

float AGameGrid::GetTileRadius() const
{
    //todo update for hex and triangles
    return TileEdgeLength / FMath::Sqrt(2.0f);
}

float AGameGrid::GetTileEdgeLength() const
{
    return TileEdgeLength;
}

bool AGameGrid::IsValidGridLocation(const FVector& InLocation) const
{
    // Check if the location is within the grid's bounding box

    bool check1 = InLocation.X >= GetActorLocation().X;
    bool check2 = InLocation.X <= GetActorLocation().X + GridLength;
    bool check3 = InLocation.Y >= GetActorLocation().Y;
    bool check4 = InLocation.Y <= GetActorLocation().Y + GridWidth;

    bool retval = check1 && check2 && check3 && check4;
    return retval;
}

void AGameGrid::SetTileVisible(int32 TileID, bool bIsVisble) const
{
    if (IsValid(SelectionProceduralMesh))
    {
        SelectionProceduralMesh->SetMeshSectionVisible(TileID, bIsVisble);
    }
}

void AGameGrid::SetTileColor(int32 TileID, FLinearColor InTileColor)
{
    UMaterialInstanceDynamic* TileMaterial = Cast<UMaterialInstanceDynamic>(SelectionProceduralMesh->GetMaterial(TileID));
    if (IsValid(TileMaterial))
    {
        TileMaterial->SetVectorParameterValue(ColorParameterName, InTileColor);
    }
}

UGridTile* AGameGrid::GetTileFromLocation(const FVector& InLocation) const
{
    UGridTile* retval = nullptr;

    if (TileShape == EGridTileType::SQUARE)
    {
        const int32 maxrows = GetMaxRows();
        const int32 maxcols = GetMaxCols();
        const FVector gridstartlocation = GetActorLocation();
        const float ydiff = (InLocation.Y - gridstartlocation.Y);
        const float xdiff = (InLocation.X - gridstartlocation.X);

        const int32 approxcol = floorf(ydiff / (TileEdgeLength));
        const int32 approxrow = floorf(xdiff / (TileEdgeLength));

        const int32 approxindex = (approxrow * maxcols) + approxcol;

        if (approxindex > INVALID_TILE_ID && approxindex < GridData.Num())
        {
            retval = GridData[approxindex];
        }
    }


    return retval;
}

UGridLayer* AGameGrid::AddGridLayer(TSubclassOf<UGridLayer> InLayerClass, const TArray<UGridTile*>& InActiveTiles, AActor* InApplicator, const bool InbDelayActivation)
{
    UGridLayer * retval = NewObject<UGridLayer>(this, InLayerClass);
    if (OpenLayerIndices.IsEmpty())
    {
        const int32 layerid = GridLayers.Emplace(retval);
        retval->SetLayerID(layerid);
    }
    else
    {
        const int32 layerid = OpenLayerIndices[0];
        OpenLayerIndices.RemoveAt(0);
        checkf(GridLayers[layerid] == nullptr, TEXT("AGameGrid::AddGridLayer : Layer Insertion Overwriting Existing Layer"));

        GridLayers[layerid] = retval;
        retval->SetLayerID(layerid);
    }

    retval->LayerInitialize(this, InActiveTiles, InApplicator);

    if (!InbDelayActivation)
    {
        SetActiveLayer(retval);
    }

    return retval;
}

void AGameGrid::FinishLayerActivation(UGridLayer* InLayer)
{
    checkf(InLayer, TEXT("AGameGrid::FinishLayerActivation, InLayer was null"))
    if (!InLayer->IsLayerActivated())
    {
        SetActiveLayer(InLayer);
    }
}

void AGameGrid::SetActiveLayer(UGridLayer* InLayer)
{
    InLayer->OnLayerActivate();
    
    /*Call this here so that it doesnt need to be included in the "super" call of OnLayerActivate*/
    if (InLayer->IsLayerVisible())
    {
        InLayer->OnShowLayer();
    }
}

void AGameGrid::DrawGridLines()
{
    TSet<FLine> gridlines = TSet<FLine>();
    for (int i = 0; i < GridData.Num(); i++)
    {
        const TSet<FLine> boundarylines = GridData[i]->GetTileBoundaryLines();
        gridlines.Append(boundarylines);
    }

    LinesProceduralMesh->ClearAllMeshSections();
    UMaterialInstanceDynamic* linematerial = UMaterialInstanceDynamic::Create(GridMaterial, this);

    linematerial->SetVectorParameterValue(ColorParameterName, LineColor);
    linematerial->SetScalarParameterValue(OpacityParameterName, LineOpacity);
    
    TArray<FVector> verticies = TArray<FVector>();
    TArray<int> tris = TArray<int>();
    int32 sectionid = 0;

    for (const FLine& Elem : gridlines)
    {
        verticies.Reset();
        tris.Reset();
        BuildLineRenderData(Elem.StartPoint, Elem.EndPoint, GridLineThickness, verticies, tris);

        LinesProceduralMesh->CreateMeshSection(sectionid, verticies, tris, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), false);
        LinesProceduralMesh->SetMaterial(sectionid, linematerial);
        sectionid++;
    }

}

void AGameGrid::DrawGridTiles()
{
    for (int32 i = 0; i < GridData.Num(); i++)
    {
        TArray<FVector> selectedverts = TArray<FVector>();
        TArray<int> selectedtris = TArray<int>();
        UGridTile * currenttile = GridData[i];
        
        if (TileShape == EGridTileType::SQUARE)
        {
            const FVector tilecenter = currenttile->GetTileCenter();
            const float halfwidth = TileEdgeLength * .5f;
            const FVector gridfv = GetActorForwardVector();

            FVector selectstart = tilecenter + (gridfv * halfwidth);
            FVector selectend = tilecenter - (gridfv * halfwidth);

            /*Draw a single fat "Line" with the width the size of the Tile*/
            BuildLineRenderData(selectstart, selectend, TileEdgeLength, selectedverts, selectedtris);


            if (SelectionProceduralMesh && GridMaterial)
            {
                const int32 tileid = currenttile->GetTileID();
                UMaterialInstanceDynamic* selectionmaterial = UMaterialInstanceDynamic::Create(GridMaterial, this);
                selectionmaterial->SetVectorParameterValue(ColorParameterName, DefaultTileFillColor);
                selectionmaterial->SetScalarParameterValue(OpacityParameterName, DefaultTileFillOpacity);

                SelectionProceduralMesh->CreateMeshSection(tileid, selectedverts, selectedtris, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), false);
                SelectionProceduralMesh->SetMaterial(tileid, selectionmaterial);
                SelectionProceduralMesh->SetMeshSectionVisible(tileid, false);
            }
        }
    }
}

bool AGameGrid::BuildGridData()
{
	bool retval = true;
    
    // Clear previous data todo: actually delete the tiles / do better cleanup
    GridData.Empty();

    // Create new grid data
    const int32 rowcount = GetMaxRows();
    const int32 colcount = GetMaxCols();

    for (int32 row = 0; row < rowcount; row++)
    {
        for (int32 col = 0; col < colcount; col++)
        {
            FVector tilecenterlocation = FVector();

            if (DetermineTileLocation(row, col, tilecenterlocation))
            {
                UGridTile* newtile = NewObject<UGridTile>(this, TileClass);
                const int32 tileid = (row * colcount) + col;
                newtile->SetupTile(tileid, tilecenterlocation);

                GridData.Emplace(newtile);
            }
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
            // Top Neighbor
            if (row < (numrows - 1))
            {
                currenttile->AddNeighbor(GridData[i + numcols]);

                //topright corner neighbor
                if (col < numcols - 1)
                {
                    currenttile->AddNeighbor(FGridTileNeighbor(GridData[i + numcols + 1], true));
                }
                //topleft corner neighbor
                if (col > 0)
                {
                    currenttile->AddNeighbor(FGridTileNeighbor(GridData[i + numcols - 1], true));
                }
            }
            // Check bottom neighbor
            if (row > 0)
            {
                currenttile->AddNeighbor(GridData[i - numcols]);
                
                //bottom right corner neighbor
                if (col < numcols - 1)
                {
                    currenttile->AddNeighbor(FGridTileNeighbor(GridData[i - numcols + 1], true));
                }
                //bottomleft corner neighbor
                if (col > 0)
                {
                    currenttile->AddNeighbor(FGridTileNeighbor(GridData[i - numcols - 1], true));
                }
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

void AGameGrid::InitializeLayers()
{
    for (int i = 0; i < StartupLayers.Num(); i++)
    {
        const UGridLayer* layercdo = StartupLayers[i].GetDefaultObject();
        const TArray<UGridTile*> defaultgridspace = layercdo->GetDefaultTileSet(this);
        AddGridLayer(StartupLayers[i], defaultgridspace, this);
    }
}

void AGameGrid::PreInitializeComponents()
{
    AActor::PreInitializeComponents();
    BuildGridData();
}

void AGameGrid::PostInitializeComponents()
{
    /*Use AActor because Navigation Datas Postinitialize is reliant on navigationsystemv1*/
	AActor::PostInitializeComponents();

    if (IsValid(LinesProceduralMesh))
    {
        DrawGridLines();
        DrawGridTiles();
        InitializeLayers();
    }
}

void AGameGrid::BeginPlay()
{
    Super::BeginPlay();
    DrawDebugData();
}

void AGameGrid::DrawDebugData()
{
    for (int i = 0; i < GridData.Num(); i++)
    {
        GridData[i]->DrawDebugData();
    }
}

bool AGameGrid::DetermineTileLocation(const int32 InRow, const int32 InCol, FVector& OutTileCenter) const
{
    const FVector actorlocation = GetActorLocation();

    // Determine the shape of the tile
    if (TileShape == EGridTileType::SQUARE)
    {
        const float halfedgelength = (0.5f * TileEdgeLength);
        OutTileCenter.X = actorlocation.X + (InRow * TileEdgeLength) + halfedgelength;
        OutTileCenter.Y = actorlocation.Y + (InCol * TileEdgeLength) + halfedgelength;
        OutTileCenter.Z = actorlocation.Z;
    }
    else if (TileShape == EGridTileType::HEXAGON)
    {
        // Calculate the distance between the center of each hexagon in the x and y direction
        const float xSpacing = (3.0f / 2.0f) * TileEdgeLength;
        const float ySpacing = TileEdgeLength * FMath::Sqrt(3.0f);

      //  OutTileCenter = actorlocation + FVector(x * xSpacing, y * ySpacing, 0);
    }
    bool retval = IsValidGridLocation(OutTileCenter);

    return retval;
}

void AGameGrid::BuildLineRenderData(const FVector LineStart, const FVector LineEnd, const float LineThickness, TArray<FVector>& Verts, TArray<int>& Tris)
{
	const float halfthickness = LineThickness * 0.5f;

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

bool AGameGrid::RemoveGridLayer(UGridLayer* InLayerToRemove)
{
    bool retval = false;
    InLayerToRemove->OnLayerDeactivate();
    const int32 id = InLayerToRemove->GetLayerID();
    checkf(InLayerToRemove == GridLayers[id], TEXT("AGameGrid::RemoveGridLayer LayerRemoved not in active set"));
    if (id == GridLayers.Num() - 1)
    {
        GridLayers.RemoveAt(id);
    }
    else
    {
        GridLayers[id] = nullptr;
        OpenLayerIndices.Emplace(id);
    }

    retval = InLayerToRemove->ConditionalBeginDestroy();
    return retval;
}

/*****Start Navigation ***/
/*Equivalent of findpath*/
FPathFindingResult AGameGrid::FindVectorField(const FNavAgentProperties& AgentProperties, const FVectorFieldQuery& Query)
{
    const ANavigationData* selfdata = Query.NavData.Get();
    AGameGrid* selfptr = const_cast<AGameGrid*>(Cast<AGameGrid>(selfdata));
    check(selfptr != nullptr);

    FPathFindingResult retval(ENavigationQueryResult::Error);
    FNavigationPath* navpath = Query.PathInstanceToFill.Get();
    FVectorFieldPath* navfieldpath = navpath ? navpath->CastPath<FVectorFieldPath>() : nullptr;

    if (navfieldpath)
    {

    }
    else
    {
        retval.Path = selfptr->CreatePathInstance<FVectorFieldPath>(Query);
        navpath = retval.Path.Get();
        navfieldpath = navpath ? navpath->CastPath<FVectorFieldPath>() : nullptr;
        
        const FVectorFieldQuery* vectorquery = static_cast<const FVectorFieldQuery*>(&Query);

        UFlowFieldSolutionLayer* potentialsolution = selfptr->GetSolutionFromQuery(*vectorquery);
        if (potentialsolution)
        {
            navfieldpath->InitSolution(potentialsolution);
            retval.Result = ENavigationQueryResult::Success;
        }
        else
        {
           UFlowFieldSolutionLayer * solutionlayer =  selfptr->BuildSolutionFromQuery(*vectorquery);
           if (solutionlayer)
           {
               navfieldpath->InitSolution(solutionlayer); 
               retval.Result = ENavigationQueryResult::Success;
           }
           else
           {
               retval.Result = ENavigationQueryResult::Fail;
           }
        }
    }

    return retval;
}

void AGameGrid::RequestNavigationRegistration()
{
    if (IsRegistered() == false && HasAnyFlags(RF_ClassDefaultObject) == false)
    {
        UVectorFieldNavigationSystem* NavSys = FNavigationSystem::GetCurrent<UVectorFieldNavigationSystem>(GetWorld());
        if (NavSys)
        {
            NavSys->RequestRegistrationDeferred(*this);
        }
    }
}

UFlowFieldSolutionLayer* AGameGrid::GetSolutionFromQuery(const FVectorFieldQuery& Query) const
{
    UFlowFieldSolutionLayer* retval = nullptr;

    for (int i = 0; i < GridLayers.Num(); i++)
    {
      if (UFlowFieldSolutionLayer* solution = Cast<UFlowFieldSolutionLayer>(GridLayers[i]))
      {
          if (solution->CanUseSolutionforQuery(Query))
          {
              retval = solution;
              break;
          }
      }
    }

    return retval;
}

UFlowFieldSolutionLayer* AGameGrid::BuildSolutionFromQuery(const FVectorFieldQuery& Query)
{
   UFlowFieldSolutionLayer* retval = nullptr;

   if (Query.IsGoalActor())
   {
       retval = Cast<UFlowFieldSolutionLayer>(AddGridLayer(UFlowFieldSolutionLayer::StaticClass(), GetTiles(), this));
       retval->SetGoalActor(Query.GetGoalActor(), Query.IsDynamicGoal);
       SetActiveLayer(retval);
   }
   else
   {
      const FVector endlocation = Query.EndLocation;
      const UGridTile* basegoaltile = GetTileFromLocation(endlocation);
      if (basegoaltile != nullptr)
      {
          retval = Cast<UFlowFieldSolutionLayer>(AddGridLayer(UFlowFieldSolutionLayer::StaticClass(), GetTiles(), this));
          retval->SetGoalTile(basegoaltile);
          SetActiveLayer(retval);
       }
    }

    return retval;
}

FPathFindingResult AGameGrid::RepathSolution(UFlowFieldSolutionLayer* InSolution, ENavPathUpdateType::Type InRepathReason)
{
    FPathFindingResult retval = FPathFindingResult();

    if (InRepathReason == ENavPathUpdateType::GoalMoved)
    {
        const UGridTile* oldgoaltile = InSolution->GetGoalTile();




        if (InSolution->IsSolutionReady())
        {
            retval.Result = ENavigationQueryResult::Success;
        }

        const UGridTile* newgoaltile = InSolution->GetGoalTile();
        FVector dumbugtest = FVector();

        if (InSolution->GetFlowVectorForTile(oldgoaltile, dumbugtest))
        {
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Repath Completed for Solution %d from Tile %d to Tile %d"), InSolution->GetLayerID(), oldgoaltile->GetTileID(), newgoaltile->GetTileID()));
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, FString::Printf(TEXT("Repath Failed for Tile %d"), oldgoaltile->GetTileID()));
        }
    }


    return retval;
}

void AGameGrid::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction)
{
    PurgeUnusedPaths();

    // INC_DWORD_STAT_BY(STAT_Navigation_ObservedPathsCount, ObservedPaths.Num());

    if (NextObservedPathsTickInSeconds >= 0.f)
    {
        NextObservedPathsTickInSeconds -= DeltaTime;
        if (NextObservedPathsTickInSeconds <= 0.f)
        {
            RepathRequests.Reserve(ObservedPaths.Num());

            for (int32 PathIndex = ObservedPaths.Num() - 1; PathIndex >= 0; --PathIndex)
            {
                if (ObservedPaths[PathIndex].IsValid())
                {
                    FNavPathSharedPtr SharedPath = ObservedPaths[PathIndex].Pin();
                    FNavigationPath* Path = SharedPath.Get();
                    EPathObservationResult::Type Result = Path->TickPathObservation();
                    switch (Result)
                    {
                    case EPathObservationResult::NoLongerObserving:
                        ObservedPaths.RemoveAtSwap(PathIndex, 1, /*bAllowShrinking=*/false);
                        break;

                    case EPathObservationResult::NoChange:
                        // do nothing
                        break;

                    case EPathObservationResult::RequestRepath:
                        RepathRequests.Add(FNavPathRecalculationRequest(SharedPath, ENavPathUpdateType::GoalMoved));
                        break;

                    default:
                        check(false && "unhandled EPathObservationResult::Type in ANavigationData::TickActor");
                        break;
                    }
                }
                else
                {
                    ObservedPaths.RemoveAtSwap(PathIndex, 1, /*bAllowShrinking=*/false);
                }
            }

            if (ObservedPaths.Num() > 0)
            {
                NextObservedPathsTickInSeconds = ObservedPathsTickInterval;
            }
        }
    }

    if (RepathRequests.Num() > 0)
    {
        float TimeStamp = GetWorldTimeStamp();
        const UWorld* World = GetWorld();

        // @todo batch-process it!

        const int32 MaxProcessedRequests = 1000;

        // make a copy of path requests and reset (remove up to MaxProcessedRequests) from navdata's array
        // this allows storing new requests in the middle of loop (e.g. used by meta path corrections)

        TArray<FNavPathRecalculationRequest> WorkQueue(RepathRequests);
        if (WorkQueue.Num() > MaxProcessedRequests)
        {
            WorkQueue.RemoveAt(MaxProcessedRequests, WorkQueue.Num() - MaxProcessedRequests);
            RepathRequests.RemoveAt(0, MaxProcessedRequests);
        }
        else
        {
            RepathRequests.Reset();
        }

        for (int32 Idx = 0; Idx < WorkQueue.Num(); Idx++)
        {
            FNavPathRecalculationRequest& RecalcRequest = WorkQueue[Idx];

            // check if it can be updated right now
            FNavPathSharedPtr PinnedPath = RecalcRequest.Path.Pin();
            if (PinnedPath.IsValid() == false)
            {
                continue;
            }

            FVectorFieldPath* path = PinnedPath->CastPath<FVectorFieldPath>();
            int32 id = path->GetSolutionID();
            UFlowFieldSolutionLayer* solution = GetLayerByIndex<UFlowFieldSolutionLayer>(id);

            const FPathFindingResult Result = RepathSolution(solution, ENavPathUpdateType::GoalMoved);



            // update time stamp to give observers any means of telling if it has changed
            PinnedPath->SetTimeStamp(TimeStamp);

            // partial paths are still valid and can change to full path when moving goal gets back on navmesh
            if (Result.IsSuccessful() || Result.IsPartial())
            {
                PinnedPath->UpdateLastRepathGoalLocation();
                PinnedPath->DoneUpdating(RecalcRequest.Reason);
                if (RecalcRequest.Reason == ENavPathUpdateType::NavigationChanged)
                {
                    RegisterActivePath(PinnedPath);
                }
            }
            else
            {
                //PinnedPath->RePathFailed();
            }
        }
    }

}

/*************End Navigation****************/

void AGameGrid::PostLoad()
{
    AActor::PostLoad();

    bNetLoadOnClient = FNavigationSystem::ShouldLoadNavigationOnClient(*this);
    RequestNavigationRegistration();
}


#ifdef WITH_EDITOR
void AGameGrid::RebuildGridData(bool bRedrawMesh)
{
    BuildGridData();
    if (bRedrawMesh && IsValid(LinesProceduralMesh))
    {
        DrawGridLines();
    }
}

void AGameGrid::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (bRebuildGridData == true)
    {
        UWorld* World = GetWorld();

        if (World != nullptr)
        {
            EWorldType::Type type = World->WorldType;
            if (type == EWorldType::Editor)
            {
                if (BuildGridData() && IsValid(LinesProceduralMesh))
                {
                    DrawGridLines();
                    DrawGridTiles();
                    bRebuildGridData = false;
                }
            }
        }


        const UWorld* world = GetWorld();
        if (IsValid(world) && world->WorldType == EWorldType::Editor)
        {
            for (TActorIterator<AGridAttachmentActor> ActorItr(world); ActorItr; ++ActorItr)
            {
                AGridAttachmentActor* found = *ActorItr;
                if (!IsValid(found->GetGameGrid()) || found->GetGameGrid() == this)
                {
                    found->OnGridConstructed(this);
                }

            }
        }
    }

}

void AGameGrid::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    const FName PropName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : FName();

    if (PropName == GET_MEMBER_NAME_CHECKED(AGameGrid, TileEdgeLength)
        || PropName == GET_MEMBER_NAME_CHECKED(AGameGrid, GridLength)
        || PropName == GET_MEMBER_NAME_CHECKED(AGameGrid, GridWidth))
    {
        if (TileEdgeLength > GridLength || TileEdgeLength > GridWidth)
        {
            if (GridLength <= GridWidth)
            {
                TileEdgeLength = GridLength;
            }
            else
            {
                TileEdgeLength = GridWidth;
            }
        }

        RebuildGridData();
    }
}
#endif