// Fill out your copyright notice in the Description page of Project Settings.
#include "FlockAIController.h"
#include "../Navigation/FlockPathFollowingComponent.h"
#include "../FlockingSystemCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Navigation/CrowdFollowingComponent.h"

AFlockAIController::AFlockAIController()
{
	SetPathFollowingComponent(CreateDefaultSubobject<UFlockPathFollowingComponent>(TEXT("PathFollowingComp")));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
}

void AFlockAIController::OnPossess(APawn* InPawn)
{
	AFlockingSystemCharacter * Minion = Cast<AFlockingSystemCharacter>(InPawn);

	if (Minion != nullptr && Minion->BehaviorTree != nullptr)
	{
		BlackboardComp->InitializeBlackboard(*Minion->BehaviorTree->BlackboardAsset);
		BehaviorComp->StartTree(*Minion->BehaviorTree);
		SetGoalActor(Minion->PreSetFlockGoal);
	}

	
}

void AFlockAIController::SetGoalActor(AActor* InGoal)
{
	if (BlackboardComp != nullptr && InGoal != nullptr)
	{
		BlackboardComp->SetValueAsObject("GoalObject", InGoal);
	}
}
