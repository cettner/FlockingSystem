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

AGameGrid* UGridLayerComponent::GetGameGrid() const
{
	AGameGrid* retval = nullptr;
	if (AGridAttachmentActor * parent = Cast<AGridAttachmentActor>(GetAttachParentActor()))
	{

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

void UGridLayerComponent::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}


