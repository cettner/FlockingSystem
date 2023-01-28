// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SquareGameGrid.h"
#include "GridModifierType.generated.h"




class AClaimableSquareGameGrid;
class AGridClaimingActor;

UCLASS()
class UGridModifierType : public UObject
{
	GENERATED_BODY()

  public:
	virtual bool IsModifierActive(FGridTileData TileLocation) const;
	void ApplyModifier(AClaimableSquareGameGrid * ParentGrid, FGridTileData TileLocation, AGridClaimingActor * Invoker);
	void ApplyModifier(AClaimableSquareGameGrid * ParentGrid, TArray<FGridTileData> TileLocations, AGridClaimingActor * Invoker);
	bool RemoveModifier(AClaimableSquareGameGrid * ParentGrid, FGridTileData TileLocation, AGridClaimingActor * Invoker);
	bool RemoveModifier(AClaimableSquareGameGrid * ParentGrid, TArray<FGridTileData> TileLocations, AGridClaimingActor * Invoker);
	virtual void RemoveAll(AClaimableSquareGameGrid * ParentGrid, AGridClaimingActor * Invoker);
	void SetSourceActor(AActor * InSource);

	AActor * GetSourceActor() const;

	template < class T >
	T*  GetSourceActor() const
	{
		return Cast<T>(GetSourceActor());
	}
  
  protected:
	  virtual void OnModifierApply(AClaimableSquareGameGrid * ParentGrid, FGridTileData TileLocation, AGridClaimingActor * Invoker);
	  virtual void OnModiferRemoved(AClaimableSquareGameGrid * ParentGrid, FGridTileData TileLocation, AGridClaimingActor * Invoker);

  protected:
	bool bIsActive = true;
	bool bCallsRemovalOnDestruction = true;

	AClaimableSquareGameGrid * WorkingGrid = nullptr;
	AActor * ModSource = nullptr;
	TArray<FGridTileData> AppliedTiles = TArray<FGridTileData>();

};
