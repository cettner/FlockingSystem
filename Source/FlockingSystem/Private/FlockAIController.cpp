// Fill out your copyright notice in the Description page of Project Settings.
#include "FlockAIController.h"
#include "../Navigation/FlockPathFollowingComponent.h"
#include "../Navigation/VectorFieldNavigationSystem.h"
#include "../FlockingSystemCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Navigation/CrowdFollowingComponent.h"

AFlockAIController::AFlockAIController()
{
	UFlockPathFollowingComponent* vectorfollowingcomponent = CreateDefaultSubobject<UFlockPathFollowingComponent>(TEXT("PathFollowingComp"));
	SetPathFollowingComponent(vectorfollowingcomponent);
	vectorfollowingcomponent->OnRequestFinished.AddUObject(this, &AAIController::OnMoveCompleted);

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
	}

	
}

FPathFollowingRequestResult AFlockAIController::MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath)
{
	FPathFollowingRequestResult ResultData;
	ResultData.Code = EPathFollowingRequestResult::Failed;

	return Super::MoveTo(MoveRequest, OutPath);
}

FAIRequestID AFlockAIController::RequestMove(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr Path)
{
	return Super::RequestMove(MoveRequest, Path);
}

void AFlockAIController::FindPathForMoveRequest(const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query, FNavPathSharedPtr& OutPath) const
{
	UVectorFieldNavigationSystem* NavSys = FNavigationSystem::GetCurrent<UVectorFieldNavigationSystem>(GetWorld());
	
	if (NavSys)
	{
		
	}
}

void AFlockAIController::SetGoalActor(AActor* InGoal)
{
	if (BlackboardComp != nullptr && InGoal != nullptr)
	{
		BlackboardComp->SetValueAsObject("GoalObject", InGoal);
	}
}
