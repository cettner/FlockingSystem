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
		//UE_VLOG(this, LogAINavigation, Log, TEXT("MoveTo: already at goal!"));
		ResultData.MoveId = GetPathFollowingComponent()->RequestMoveWithImmediateFinish(EPathFollowingResult::Success);
		ResultData.Code = EPathFollowingRequestResult::AlreadyAtGoal;
	}
	else
	{
		FPathFindingQuery PFQuery;
		const bool bValidQuery = BuildPathfindingQuery(MoveRequest, PFQuery);

		if (bValidQuery)
		{
			FNavPathSharedPtr Path;
			FindPathForMoveRequest(MoveRequest, PFQuery, Path);

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

void AFlockAIController::FindPathForMoveRequest(const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query, FNavPathSharedPtr& OutPath) const
{
	UVectorFieldNavigationSystem* NavSys = FNavigationSystem::GetCurrent<UVectorFieldNavigationSystem>(GetWorld());
	
	if (NavSys)
	{
		FPathFindingResult PathResult = NavSys->FindPathSync(Query);
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
	}
}

void AFlockAIController::SetGoalLocation(FVector InLocation)
{
	if (BlackboardComp != nullptr)
	{
		BlackboardComp->SetValueAsVector("GoalLocation", InLocation);
	}
}

bool AFlockAIController::BuildPathfindingQuery(const FAIMoveRequest& MoveRequest, FPathFindingQuery& OutQuery) const
{
	bool bResult = false;

	UVectorFieldNavigationSystem* NavSys = FNavigationSystem::GetCurrent<UVectorFieldNavigationSystem>(GetWorld());
	const ANavigationData* NavData = (NavSys == nullptr) ? nullptr : NavSys->GetNavDataForProps(GetNavAgentPropertiesRef(), GetNavAgentLocation());


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

		FSharedConstNavQueryFilter NavFilter = UNavigationQueryFilter::GetQueryFilter(*NavData, this, MoveRequest.GetNavigationFilter());
		OutQuery = FPathFindingQuery(*this, *NavData, GetNavAgentLocation(), GoalLocation, NavFilter);
		OutQuery.SetAllowPartialPaths(MoveRequest.IsUsingPartialPaths());

		if (GetPathFollowingComponent())
		{
			GetPathFollowingComponent()->OnPathfindingQuery(OutQuery);
		}

		bResult = true;
	}


	return bResult;
}
