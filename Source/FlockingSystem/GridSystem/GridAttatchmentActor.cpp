// Fill out your copyright notice in the Description page of Project Settings.


#include "GridAttatchmentActor.h"

#include "EngineUtils.h"

// Sets default values
AGridAttatchmentActor::AGridAttatchmentActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
}

void AGridAttatchmentActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	bool attatchgridonentry = false;
	if (GetWorld())
	{
		EWorldType::Type worldtype = GetWorld()->WorldType;
		attatchgridonentry = (worldtype != EWorldType::EditorPreview);
	}

	if (attatchgridonentry)
	{
		SetParentGrid(AttachToGrid(GetActorLocation()));

		if (!bSavedOffsets)
		{
			/*Save the Original Local Offsets for all Prmitive Components*/
			SavePrimitiveOffsets();
			bSavedOffsets = SavedLocalOffsets.Num() > 0;
		}

		UpdatePrimatives();
	}
}

ASquareGameGrid * AGridAttatchmentActor::AttachToGrid(FVector StartLocation, ASquareGameGrid * InGrid)
{
	ASquareGameGrid * foundgrid = nullptr;

	if (InGrid)
	{
		foundgrid = InGrid;

		FGridTileData foundtile = foundgrid->GetTileFromLocation(StartLocation);
		if (foundtile.IsValid)
		{
			SetActorLocation(foundtile.TileCenter);
			RootGridLocation = foundtile;
		}

	}
	else
	{
		foundgrid = FindGrid();
		
		if (foundgrid != nullptr)
		{
			FGridTileData foundtile = foundgrid->GetTileFromLocation(StartLocation);
			if (foundtile.IsValid)
			{
				SetActorLocation(foundtile.TileCenter);
				RootGridLocation = foundtile;
			}
		}

	}

	return (foundgrid);
}

void AGridAttatchmentActor::UpdatePrimatives()
{
	if (GetParentGrid() == nullptr) return;

	for (TPair<UPrimitiveComponent *, FVector> Elem : SavedLocalOffsets)
	{
		if (Elem.Key != nullptr)
		{
			FVector AttemptedTileLocation = GetActorLocation() + Elem.Value;
			FGridTileData foundtile = GetParentGrid()->GetTileFromLocation(AttemptedTileLocation);

			if (foundtile.IsValid)
			{
				Elem.Key->SetWorldLocation(foundtile.TileCenter);
				PrimativeTileLocations.Add(Elem.Key, foundtile);
			}
			else
			{
				Elem.Key->SetWorldLocation(GetActorLocation() + Elem.Value);
				PrimativeTileLocations.Add(Elem.Key, foundtile);
			}
		}
	}
}

bool AGridAttatchmentActor::SetTileLocation(FGridTileData Moveto)
{
	if (ParentGrid == nullptr || !Moveto.IsValid) return false;

	FVector movelocation = GetActorLocation();
		
	bool success = ParentGrid->GetLocationFromTile(Moveto, movelocation);
	
	if (success)
	{
		SetActorLocation(movelocation);
		SetRootGridTile(Moveto);
		UpdatePrimatives();
	}

	return(success);
}

ASquareGameGrid * AGridAttatchmentActor::FindGrid() const
{
	UWorld * World = GetWorld();

	if (World == nullptr) return nullptr;

	for (TActorIterator<ASquareGameGrid> ActorItr(World); ActorItr; ++ActorItr)
	{
		ASquareGameGrid * found = *ActorItr;
		return(found);
	}


	return nullptr;
}

void AGridAttatchmentActor::SetParentGrid(ASquareGameGrid * InGrid)
{
	ParentGrid = InGrid;
}

ASquareGameGrid * AGridAttatchmentActor::GetParentGrid() const
{
	return ParentGrid;
}

FGridTileData AGridAttatchmentActor::GetRootGridTile() const
{
	return RootGridLocation;
}

bool AGridAttatchmentActor::CanBePlacedAt(FGridTileData TestTile) const
{
	if (!TestTile.IsValid) return false;

	bool alltilesvalid = true;

	for (TPair<UPrimitiveComponent *, FVector> Elem : SavedLocalOffsets)
	{
		if (Elem.Key != nullptr)
		{
			FVector AttemptedTileLocation = TestTile.TileCenter + Elem.Value;
			FGridTileData foundtile = GetParentGrid()->GetTileFromLocation(AttemptedTileLocation);
			alltilesvalid |= foundtile.IsValid;
		}
	}

	return (alltilesvalid);
}

void AGridAttatchmentActor::SavePrimitiveOffsets()
{
	TArray<UPrimitiveComponent *> comps = TArray<UPrimitiveComponent *>();
	GetComponents< UPrimitiveComponent>(comps);

	for (int i = 0; i < comps.Num(); i++)
	{
		UPrimitiveComponent * primitive = comps[i];

		if (primitive != nullptr)
		{
			SavedLocalOffsets.Add(primitive, primitive->GetRelativeLocation());
		}

	}
}

void AGridAttatchmentActor::SetRootGridTile(FGridTileData InTile)
{
	RootGridLocation = InTile;
}


#ifdef WITH_EDITOR
void AGridAttatchmentActor::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);
	bool attatchgridonentry = false;
	if (GetWorld())
	{
		EWorldType::Type worldtype = GetWorld()->WorldType;
		attatchgridonentry = (worldtype != EWorldType::EditorPreview);
	}

	if (attatchgridonentry)
	{
		SetParentGrid(AttachToGrid(GetActorLocation(), ParentGrid));

		if (!bSavedOffsets)
		{
			/*Save the Original Local Offsets for all Prmitive Components*/
			SavePrimitiveOffsets();
			bSavedOffsets = SavedLocalOffsets.Num() > 0;
		}

		UpdatePrimatives();
	}
}
#endif