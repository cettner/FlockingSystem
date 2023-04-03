// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"

#include "GridLayer.h"
#include "GridLayerComponent.generated.h"


UCLASS()
class FLOCKINGSYSTEM_API UGridLayerComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGridLayerComponent();

protected:

		

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGridLayer> LayerClass = nullptr;
};
