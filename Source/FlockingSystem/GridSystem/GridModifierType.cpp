// Fill out your copyright notice in the Description page of Project Settings.


#include "GridModifierType.h"
#include "GridClaimingActor.h"
#include "ClaimableSquareGameGrid.h"





bool UGridModifierType::IsModifierActive(FGridTileData TileLocation) const
{
	return AppliedTiles.Contains(TileLocation);
}

void UGridModifierType::ApplyModifier(AClaimableSquareGameGrid * ParentGrid, FGridTileData TileLocation, AGridClaimingActor * Invoker)
{
	if (TileLocation.IsValid && !AppliedTiles.Contains(TileLocation))
	{
		AppliedTiles.Emplace(TileLocation);
		WorkingGrid = ParentGrid;

		OnModifierApply(ParentGrid, TileLocation, Invoker);
	}
}

void UGridModifierType::ApplyModifier(AClaimableSquareGameGrid * ParentGrid, TArray<FGridTileData> TileLocations, AGridClaimingActor * Invoker)
{
	for (int i = 0; i < TileLocations.Num(); i++)
	{
		ApplyModifier(ParentGrid, TileLocations[i], Invoker);
	}
}

bool UGridModifierType::RemoveModifier(AClaimableSquareGameGrid * ParentGrid, FGridTileData TileLocation, AGridClaimingActor * Invoker)
{
	bool retval = false;
	int32 index = AppliedTiles.IndexOfByKey(TileLocation);
	if (index > INDEX_NONE)
	{
		AppliedTiles.RemoveAt(index);

		OnModiferRemoved(ParentGrid,TileLocation,Invoker);
	}

	retval = index > INDEX_NONE;
	return(retval);
}

bool UGridModifierType::RemoveModifier(AClaimableSquareGameGrid * ParentGrid, TArray<FGridTileData> TileLocations, AGridClaimingActor * Invoker)
{
	bool retval = true;
	for (int i = 0; i < TileLocations.Num(); i++)
	{
		retval &= RemoveModifier(ParentGrid, TileLocations[i], Invoker);
	}

	return(retval);
}

void UGridModifierType::RemoveAll(AClaimableSquareGameGrid * ParentGrid, AGridClaimingActor * Invoker)
{
		RemoveModifier(ParentGrid, AppliedTiles, Invoker);
}

void UGridModifierType::SetSourceActor(AActor * InSource)
{
	ModSource = InSource;
}

AActor * UGridModifierType::GetSourceActor() const
{
	return ModSource;
}

void UGridModifierType::OnModifierApply(AClaimableSquareGameGrid * ParentGrid, FGridTileData TileLocation, AGridClaimingActor * Invoker)
{

}

void UGridModifierType::OnModiferRemoved(AClaimableSquareGameGrid * ParentGrid, FGridTileData TileLocation, AGridClaimingActor * Invoker)
{

}

