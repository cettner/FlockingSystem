// Fill out your copyright notice in the Description page of Project Settings.


#include "FlockTestLevelDriver.h"
#include "../GridSystem/GameGrid.h"
#include "../GridSystem/Layers/FlowFieldIntegrationLayer.h"
#include "../GridSystem/Layers/FlowFieldVectorLayer.h"
#include "../GridSystem/Layers/FlowFieldSolutionLayer.h"
#include "../FlockingSystemCharacter.h"

#include "AIController.h"
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
				FVector testlocationmove = gridtiles[0]->GetTileCenter();
				solution->AddGoalTile(gridtiles[0]);
				GameGrid->SetActiveLayer(solution);
				solution->SetLayerVisibility(true);
			}


			for (TObjectIterator<AFlockingSystemCharacter> charItr; charItr; ++charItr)
			{
				AFlockingSystemCharacter* navagent = *charItr;
				AAIController* controller = navagent->GetController<AAIController>();

				if (controller != nullptr && gridtiles.Num())
				{
					controller->MoveToLocation(FVector(100.0, 100.0, 10.0));
				}
			}
		}




	}
}
