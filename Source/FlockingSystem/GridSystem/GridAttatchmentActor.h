// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SquareGameGrid.h"
#include "GridAttatchmentActor.generated.h"








UCLASS()
class AGridAttatchmentActor : public AActor
{
	GENERATED_BODY()
	

public:	
	// Sets default values for this actor's properties
	AGridAttatchmentActor();

public:
	
	/*Attmpts to Attatch the Actor to the grid at the specified location, Assigns Grid Location and Parent Grid*/
	virtual ASquareGameGrid * AttachToGrid(FVector StartLocation, ASquareGameGrid * InGrid = nullptr);
	
	/*Moves the Actor and all primitive componenets to the specified tile if Valid*/
	virtual bool SetTileLocation(FGridTileData Moveto);

	/*Returns Currently Connected Grid*/
	ASquareGameGrid * GetParentGrid() const;

	template < class T >
	T* GetParentGrid() const
	{
		return Cast<T>(GetParentGrid());
	}

	
	/*Gets the Tile the root component is currently occupying*/
	FGridTileData GetRootGridTile() const;

	/*Tests whether the Actor and all registered primitives can be placed at the target location and remain on the Grid return false if any primitaves or root tile would land on invalid tiles*/
	virtual bool CanBePlacedAt(FGridTileData TestTile) const;


	virtual ASquareGameGrid* FindGrid() const;

protected:
	virtual void SetParentGrid(ASquareGameGrid * InGrid);
	virtual void SavePrimitiveOffsets();
	virtual void SetRootGridTile(FGridTileData InTile);


	/*Updates Registered Primative Components based on thier relative location to Grid Space Sets normal Relative Offset if tile is invalid*/
	virtual void UpdatePrimatives();

protected:
	virtual void PostInitializeComponents() override;


protected:
	ASquareGameGrid * ParentGrid = nullptr;
	FGridTileData RootGridLocation = FGridTileData();
	
	bool bSavedOffsets = false;
	TMap<UPrimitiveComponent *, FVector> SavedLocalOffsets = TMap<UPrimitiveComponent *, FVector>();
	TMap<UPrimitiveComponent *, FGridTileData> PrimativeTileLocations = TMap <UPrimitiveComponent *, FGridTileData>();



#ifdef WITH_EDITOR
protected:
	virtual void OnConstruction(const FTransform & Transform) override;
#endif



};
