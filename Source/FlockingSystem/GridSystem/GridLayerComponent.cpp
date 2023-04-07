// Fill out your copyright notice in the Description page of Project Settings.


#include "GridLayerComponent.h"
#include "GridAttachmentActor.h"

// Sets default values for this component's properties
UGridLayerComponent::UGridLayerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

TArray<UGridTile*> UGridLayerComponent::GetGridSpace() const
{
	return TArray<UGridTile*>();
}

void UGridLayerComponent::ApplyLayers()
{
	AGameGrid * grid = GetGameGrid();
	TArray<UGridTile*> gridspace = GetGridSpace();
	AActor * applicator = GetAttachParentActor();

	for (int i = 0; i < LayerClasses.Num(); i++)
	{
		if (LayerClasses[i] != nullptr)
		{
			UGridLayer * layer = grid->AddGridLayer(LayerClasses[i], gridspace , applicator);
		}
	}
}

AGameGrid* UGridLayerComponent::GetGameGrid() const
{
	AGameGrid* retval = nullptr;
	if (AGridAttachmentActor * parent = Cast<AGridAttachmentActor>(GetAttachParentActor()))
	{
		retval = parent->GetGameGrid();
	}

	return retval;
}

void UGridLayerComponent::OnRegister()
{
	Super::OnRegister();


}

void UGridLayerComponent::PostEditComponentMove(bool bFinished)
{
	Super::PostEditComponentMove(bFinished);

	if (bFinished == true)
	{
		GetComponentLocation();

	}
}

void UGridLayerComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}


