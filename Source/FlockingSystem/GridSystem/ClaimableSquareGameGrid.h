// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SquareGameGrid.h"
#include "ClaimableSquareGameGrid.generated.h"


/*Forward Declaration*/
class AGridClaimingActor;
class UGridModifierType;
struct FGridTileOffset;

USTRUCT()
struct FGridModifierSet
{
	GENERATED_USTRUCT_BODY()
	TArray<UGridModifierType*> TileModifiers = TArray<UGridModifierType*>();
};



UCLASS()
class AClaimableSquareGameGrid : public ASquareGameGrid
{
	GENERATED_BODY()

public:
	  virtual bool AddGridActor(AGridClaimingActor * InActor, FGridTileData TileLocation, bool TickOnAdd = true);
	  virtual bool RemoveGridActor(AGridClaimingActor * InActor, bool TickOnRemoval = false);
	  virtual bool MoveGridActor(AGridClaimingActor * InActor, FGridTileData TileLocation);
	  virtual bool CanMoveTo(AGridClaimingActor * InActor, FGridTileData TileLocation);
	  
	  virtual TArray<UGridModifierType *> GetActiveModifiers(FGridTileData TileLocation) const;
	  
	  virtual bool ApplyModifier(UGridModifierType * Modifier, FGridTileData TileLocation,  AGridClaimingActor * Source);
	  virtual bool ApplyModifier( UGridModifierType * Modifier, TArray<FGridTileData> TileLocations, AGridClaimingActor * Source);
	  virtual bool RemoveModifier(UGridModifierType * Modifier, FGridTileData TileLocation,  AGridClaimingActor * Source);
	  virtual bool RemoveModifier(UGridModifierType * Modifier, TArray<FGridTileData> TileLocations,  AGridClaimingActor* Source);

protected:
	virtual bool BuildGridData() override;

protected:
	virtual bool GetGridTilesFromOffset(FGridTileData StartTile, TArray<FGridTileOffset> Offsets,  TArray<FGridTileData> & OutTiles, bool bisstartinclusive = false) const;



protected:
	TArray<AGridClaimingActor *> GridActors;
	TArray<FGridModifierSet> GridModifiers;





#if WITH_EDITOR
public:
	bool ISSimulatingEffects() const;
	virtual void SimulateGrid();

protected:
	virtual void OnConstruction(const FTransform & Transform) override;
#endif
};
