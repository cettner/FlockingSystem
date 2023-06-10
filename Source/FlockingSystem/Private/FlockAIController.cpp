// Fill out your copyright notice in the Description page of Project Settings.
#include "FlockAIController.h"
#include "../Navigation/FlockPathFollowingComponent.h"
#include "../Navigation/VectorFieldNavigationSystem.h"
#include "../FlockingSystemCharacter.h"

#include "VisualLogger/VisualLogger.h"
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
	Super::OnPossess(InPawn);

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

	bool bCanRequestMove = true;
	const bool bAlreadyAtGoal = bCanRequestMove && GetPathFollowingComponent()->HasReached(MoveRequest);

	if (bAlreadyAtGoal)
	{
		ResultData.MoveId = GetPathFollowingComponent()->RequestMoveWithImmediateFinish(EPathFollowingResult::Success);
		ResultData.Code = EPathFollowingRequestResult::AlreadyAtGoal;
	}
	else
	{
		FVectorFieldQuery VFQuery;
		const bool bValidQuery = BuildPathfindingQuery(MoveRequest, VFQuery);

		if (bValidQuery)
		{
			FNavPathSharedPtr Path;
			FindFieldForMoveRequest(MoveRequest, VFQuery, Path);

			const FAIRequestID RequestID = Path.IsValid() ? RequestMove(MoveRequest, Path) : FAIRequestID::InvalidRequest;
			if (RequestID.IsValid())
			{
				bAllowStrafe = MoveRequest.CanStrafe();
				ResultData.MoveId = RequestID;
				ResultData.Code = EPathFollowingRequestResult::RequestSuccessful;

				if (OutPath)
				{
					*OutPath = Path;
				}
			}
		}

	}

	return ResultData;
}

FAIRequestID AFlockAIController::RequestMove(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr Path)
{
	uint32 RequestID = FAIRequestID::InvalidRequest;
	if (GetPathFollowingComponent())
	{
		RequestID = GetPathFollowingComponent()->RequestMove(MoveRequest, Path);
	}

	return RequestID;
}

void AFlockAIController::FindFieldForMoveRequest(const FAIMoveRequest& MoveRequest, FVectorFieldQuery& Query, FNavPathSharedPtr& OutPath) const
{
	UVectorFieldNavigationSystem* NavSys = FNavigationSystem::GetCurrent<UVectorFieldNavigationSystem>(GetWorld());
	
	if (NavSys)
	{
		FPathFindingResult PathResult = NavSys->FindVectorFieldPathSync(Query);
		if (PathResult.Result != ENavigationQueryResult::Error)
		{
			if (PathResult.IsSuccessful() && PathResult.Path.IsValid())
			{
				if (MoveRequest.IsMoveToActorRequest())
				{
					PathResult.Path->SetGoalActorObservation(*MoveRequest.GetGoalActor(), 100.0f);
				}

				PathResult.Path->EnableRecalculationOnInvalidation(true);
				OutPath = PathResult.Path;
			}
		}
	}
}

void AFlockAIController::SetGoalActor(AActor* InGoal)
{
	if (BlackboardComp != nullptr && InGoal != nullptr)
	{
		BlackboardComp->SetValueAsObject("GoalActor", InGoal);
		BlackboardComp->ClearValue("GoalLocation");
	}

}

void AFlockAIController::SetGoalLocation(FVector InLocation)
{
	if (BlackboardComp != nullptr)
	{
		BlackboardComp->SetValueAsVector("GoalLocation", InLocation);
		BlackboardComp->ClearValue("GoalActor");
	}
}

bool AFlockAIController::BuildPathfindingQuery(const FAIMoveRequest& MoveRequest, FVectorFieldQuery& OutQuery) const
{
	bool bResult = false;

	UVectorFieldNavigationSystem* NavSys = FNavigationSystem::GetCurrent<UVectorFieldNavigationSystem>(GetWorld());
	const AGameGrid* NavData = (NavSys == nullptr) ? nullptr : Cast<AGameGrid>(NavSys->GetNavDataForProps(GetNavAgentPropertiesRef(), GetNavAgentLocation()));


	if (NavData)
	{
		FVector GoalLocation = MoveRequest.GetGoalLocation();
		if (MoveRequest.IsMoveToActorRequest())
		{
			const INavAgentInterface* NavGoal = Cast<const INavAgentInterface>(MoveRequest.GetGoalActor());
			if (NavGoal)
			{
				const FVector Offset = NavGoal->GetMoveGoalOffset(this);
				GoalLocation = FQuatRotationTranslationMatrix(MoveRequest.GetGoalActor()->GetActorQuat(), NavGoal->GetNavAgentLocation()).TransformPosition(Offset);
			}
			else
			{
				GoalLocation = MoveRequest.GetGoalActor()->GetActorLocation();
			}
		}

		OutQuery = FVectorFieldQuery(*this, *NavData, GetNavAgentLocation(), GoalLocation);
		OutQuery.IsDynamicGoal = MoveRequest.GetGoalActor() != nullptr;
		OutQuery.SetGoalActor(MoveRequest.GetGoalActor());

		bResult = true;
	}


	return bResult;
}
