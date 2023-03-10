// Fill out your copyright notice in the Description page of Project Settings.


#include "GridClaimingActor.h"
#include "ClaimableSquareGameGrid.h"

AGridClaimingActor::AGridClaimingActor() : Super()
{
	ClaimSpaceComp = CreateDefaultSubobject<UBoxComponent>(TEXT("ClaimSpaceHelper"));
	if (ClaimSpaceComp)
	{
		ClaimSpaceComp->SetCanEverAffectNavigation(false);
	}

}

void AGridClaimingActor::PostInitializeComponents()
{
	InitializeModifiers();
	Super::PostInitializeComponents();
	PostTileChange(GetRootGridTile());
}

void AGridClaimingActor::BeginDestroy()
{
	Super::BeginDestroy();
	AClaimableSquareGameGrid* pgrid = GetParentGrid<AClaimableSquareGameGrid>();

	if (pgrid && pgrid->RemoveGridActor(this))
	{
		Modifiers.Empty();
	}

}

void AGridClaimingActor::InitializeClaimSpace(ASquareGameGrid * InGrid)
{
	if (!InGrid || !ClaimSpaceComp) return;
	RelativeClaimSpace.Empty();

	FVector extent = ClaimSpaceComp->GetUnscaledBoxExtent();
	float halftile = InGrid->GetTileSize() / 2.0f;

	int verticaltiles = roundf((extent.X + halftile) / InGrid->GetTileSize()) - 1U;
	int horizontaltiles = roundf((extent.Y + halftile) / InGrid->GetTileSize()) - 1U;

	FGridTileOffset LoadTile = FGridTileOffset();

	for (int i = -verticaltiles; i <= verticaltiles; i++)
	{
		for (int j = -horizontaltiles; j <= horizontaltiles; j++)
		{
			LoadTile.ColOffset = j;
			LoadTile.RowOffset = i;
			RelativeClaimSpace.AddUnique(LoadTile);
		}
	}
}

void AGridClaimingActor::InitializeModifiers()
{
	UWorld* world = GetWorld();
	if (world == nullptr) return;

	Modifiers.Empty();

	for (int j = 0; j < ModifierClasses.Num(); j++)
	{
		if (IsValid(ModifierClasses[j]))
		{
			UGridModifierType* newmod = NewObject<UGridModifierType>(UGridModifierType::StaticClass(), ModifierClasses[j]);
			if (newmod != nullptr)
			{
				newmod->SetSourceActor(this);
				Modifiers.Add(newmod);
			}
		}
	}
}

TArray<UGridModifierType *> AGridClaimingActor::GetActiveModifiers(FGridTileData TileData)
{
	return Modifiers;
}

void AGridClaimingActor::PreTileChange(FGridTileData NewTile)
{
	AClaimableSquareGameGrid * claimgrid = Cast<AClaimableSquareGameGrid>(GetParentGrid());
	if (claimgrid == nullptr || !NewTile.IsValid) return;

	for (int j = 0; j < Modifiers.Num(); j++)
	{
		claimgrid->RemoveModifier(Modifiers[j], GridClaimSpace, this);
	}

	GridClaimSpace.Empty();
}

void AGridClaimingActor::PostTileChange(FGridTileData NewTile, FGridTileData PrevTile)
{
	AClaimableSquareGameGrid * claimgrid = GetParentGrid<AClaimableSquareGameGrid>();
	if (claimgrid == nullptr) return;

		for (int j = 0; j < Modifiers.Num(); j++)
		{
			 claimgrid->ApplyModifier( Modifiers[j], GridClaimSpace, this);
		}
}

ASquareGameGrid * AGridClaimingActor::AttachToGrid(FVector StartLocation, ASquareGameGrid * InGrid)
{
	ASquareGameGrid * foundgrid = Super::AttachToGrid(StartLocation, InGrid);
	InitializeClaimSpace(foundgrid);

	AClaimableSquareGameGrid * claimgrid = Cast<AClaimableSquareGameGrid>(foundgrid);

	if (claimgrid)
	{
		claimgrid->AddGridActor(this, GetRootGridTile());
	}

	return(claimgrid);
}

bool AGridClaimingActor::SetTileLocation(FGridTileData Moveto)
{
	bool success = false;
	AClaimableSquareGameGrid * claimgrid = Cast<AClaimableSquareGameGrid>(GetParentGrid());
	if (claimgrid && claimgrid->CanMoveTo(this,Moveto))
	{
		FGridTileData prev = GetRootGridTile();
		PreTileChange(Moveto);
		Super::SetTileLocation(Moveto);
		success = claimgrid->MoveGridActor(this, Moveto);
		PostTileChange(Moveto, prev);
	}

	return success;
}

TArray<FGridTileOffset> AGridClaimingActor::GetRelativeClaimSpace() const
{
	return RelativeClaimSpace;
}

void AGridClaimingActor::SetGridClaimSpace(TArray<FGridTileData> ClaimedTiles, ASquareGameGrid * OwningGrid)
{
	GridClaimSpace = ClaimedTiles;
}

TArray<FGridTileData> AGridClaimingActor::GetGridClaimSpace() const
{
	return GridClaimSpace;
}

void AGridClaimingActor::SetBoxExtent(FVector Extent)
{
	ClaimSpaceComp->SetBoxExtent(Extent);

	if (GetParentGrid() == nullptr) return;
	InitializeClaimSpace(GetParentGrid());
	PostTileChange(GetRootGridTile());
}

void AGridClaimingActor::SimulateModfiers()
{
	InitializeModifiers();

	AClaimableSquareGameGrid * claimgrid = GetParentGrid<AClaimableSquareGameGrid>();
	for (int j = 0; j < Modifiers.Num(); j++)
	{
		claimgrid->ApplyModifier( Modifiers[j], GridClaimSpace, this);
	}

	Modifiers.Empty();
}


#if WITH_EDITOR
void AGridClaimingActor::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);
	AClaimableSquareGameGrid* claimgrid = Cast<AClaimableSquareGameGrid>(GetParentGrid());
	if (claimgrid && claimgrid->ISSimulatingEffects())
	{
		claimgrid->SimulateGrid();
	}
}

void AGridClaimingActor::PostEditChangeProperty(FPropertyChangedEvent& PropChange)
{
	InitializeModifiers();
	Super::PostEditChangeProperty(PropChange);
	PostTileChange(GetRootGridTile());

	AClaimableSquareGameGrid* claimgrid = Cast<AClaimableSquareGameGrid>(GetParentGrid());
	if (claimgrid && claimgrid->ISSimulatingEffects())
	{
		claimgrid->SimulateGrid();
	}
}
#endif