// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GridTile.h"

#include "GridLayer.generated.h"


constexpr int32 INVALID_LAYER_ID = 0xFFFFFFFF;

UCLASS()
class FLOCKINGSYSTEM_API UGridLayer : public UObject
{
	GENERATED_BODY()

	friend class AGameGrid;

public:
	int32 GetLayerID() const { return LayerID; }
	bool ShouldActivateAtStart() const { return bActivatesOnStartup;  }
	virtual void SetLayerVisibility(bool InbIsVisible);
	bool IsLayerVisible() const { return bIsLayerVisible; };

protected:
	virtual void LayerInitialize(AGameGrid* InGrid);
	void SetLayerID(const int32 InID) { LayerID = InID;}
	AGameGrid* GetGameGrid() const { return ParentGrid; }
	
	template<class T>
	T* GetGameGrid() const { Cast<T>(GetGameGrid()); }

	virtual void OnLayerActivate(TArray<UGridTile *> TileSubset = TArray<UGridTile*>());
	virtual uint32 OnLayerDeactivate(TArray<UGridTile*> TileSubset = TArray<UGridTile*>());
	
	virtual void OnShowLayer();
	virtual void OnHideLayer();

	virtual void PostActivateTile(UGridTile* InTile);
	virtual void PostDeactivateTile(UGridTile* InTile);
	virtual void ShowTile(UGridTile* InTile);
	virtual void HideTile(UGridTile* InTile);


protected:
	UPROPERTY(EditDefaultsOnly)
	bool bActivatesOnStartup = true;

	UPROPERTY(EditDefaultsOnly)
	bool bIsLayerVisible = false;

protected:
	TArray<UGridTile*> ActiveTiles = TArray<UGridTile*>();

protected:
	AGameGrid* ParentGrid = nullptr;
	
	int32 LayerID = INVALID_LAYER_ID;


};
