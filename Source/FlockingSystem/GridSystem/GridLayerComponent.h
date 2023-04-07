// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"


#include "GridLayer.h"
#include "GameGrid.h"
#include "GridLayerComponent.generated.h"


UCLASS()
class FLOCKINGSYSTEM_API UGridLayerComponent : public UBoxComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGridLayerComponent();
	virtual TArray<UGridTile*> GetGridSpace() const;
	virtual void ApplyLayers();

protected:
	virtual AGameGrid* GetGameGrid() const;

protected:
	virtual void OnRegister() override;

#if WITH_EDITOR
protected:
	virtual void PostEditComponentMove(bool bFinished) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif


protected:
	UPROPERTY(EditDefaultsOnly, Category = "Layers")
	TArray<TSubclassOf<UGridLayer>> LayerClasses = TArray<TSubclassOf<UGridLayer>>();

protected:
	UPROPERTY(Transient)
	TArray<UGridLayer*> ActiveLayers = TArray<UGridLayer*>();
};
