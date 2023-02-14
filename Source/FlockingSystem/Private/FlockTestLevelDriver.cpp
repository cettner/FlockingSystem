// Fill out your copyright notice in the Description page of Project Settings.


#include "FlockTestLevelDriver.h"
#include "../GridSystem/GameGrid.h"
#include "../GridSystem/Layers/FlowFieldIntegrationLayer.h"
#include "../GridSystem/Layers/FlowFieldVectorLayer.h"
#include "../GridSystem/Layers/FlowFieldSolutionLayer.h"

#include "EngineUtils.h"

void AFlockTestLevelDriver::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		UWorld* world = GetWorld();
		for (TObjectIterator<AGameGrid> Itr; Itr; ++Itr)
		{
			AGameGrid * GameGrid = *Itr;
			TArray<UGridTile*> gridtiles = GameGrid->GetTiles();
			UFlowFieldSolutionLayer* solution = Cast<UFlowFieldSolutionLayer>(GameGrid->AddGridLayer(UFlowFieldSolutionLayer::StaticClass()));
			
			if (IsValid(solution) && gridtiles.Num())
			{
				solution->AddGoalTile(gridtiles[0]);
				GameGrid->SetActiveLayer(solution);
				solution->SetLayerVisibility(true);
			}
		}
	}
}
