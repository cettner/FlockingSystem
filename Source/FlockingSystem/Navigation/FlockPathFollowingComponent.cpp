// Fill out your copyright notice in the Description page of Project Settings.


#include "FlockPathFollowingComponent.h"
#include "../GridSystem/GameGrid.h"

#include "AIController.h"

FVector UFlockPathFollowingComponent::GetFlockAgentLocation() const
{
	FVector retval = FVector::ZeroVector;
	APawn * pawn = GetOwner<AAIController>()->GetPawn();

	if (pawn)
	{
		retval = pawn->GetActorLocation();
	}

	return retval;
}

FVector UFlockPathFollowingComponent::GetFlockAgentDirection() const
{
	FVector retval = FVector::ZeroVector;
	APawn* pawn = GetOwner<AAIController>()->GetPawn();

	if (pawn)
	{
		retval = pawn->GetActorForwardVector();
	}

	return retval;
}

void UFlockPathFollowingComponent::UpdatePathSegment()
{
#if !UE_BUILD_SHIPPING
	DEBUG_bMovingDirectlyToGoal = false;
#endif // !UE_BUILD_SHIPPING

	if ((Path.IsValid() == false) || (MovementComp == nullptr))
	{
		//UE_CVLOG(Path.IsValid() == false, this, LogPathFollowing, Log, TEXT("Aborting move due to not having a valid path object"));
		OnPathFinished(EPathFollowingResult::Aborted, FPathFollowingResultFlags::InvalidPath);
		return;
	}

	const FVector CurrentLocation = MovementComp->GetActorFeetLocation();
	const bool bCanUpdateState = HasMovementAuthority();

	if (!Path->IsValid())
	{
		if (!Path->IsWaitingForRepath())
		{
			//UE_VLOG(this, LogPathFollowing, Log, TEXT("Aborting move due to path being invalid and not waiting for repath"));
			OnPathFinished(EPathFollowingResult::Aborted, FPathFollowingResultFlags::InvalidPath);
			return;
		}
		else if (HasStartedNavLinkMove() && bCanUpdateState && Status == EPathFollowingStatus::Moving)
		{
			// pawn needs to get off navlink to unlock path updates (AAIController::ShouldPostponePathUpdates)
			if (HasReachedCurrentTarget(CurrentLocation))
			{
				OnSegmentFinished();
				SetNextMoveSegment();
			}
		}
		else
		{
			// continue with execution, if navigation is being rebuild constantly AI will get stuck with current waypoint
			// path updates should be still coming in, even though they get invalidated right away
			//UE_VLOG(this, LogPathFollowing, Log, TEXT("Updating path points in invalid & pending path!"));
		}
	} 

	/** it's possible that finishing this move request will result in another request
	 *	which won't be easily detectable from this function. This simple local
	 *	variable gives us this knowledge. */
	const FAIRequestID MoveRequestId = GetCurrentRequestId();

	// if agent has control over its movement, check finish conditions
	if (bCanUpdateState && Status == EPathFollowingStatus::Moving)
	{
		const int32 LastSegmentEndIndex = Path->GetPathPoints().Num() - 1;
		const bool bFollowingLastSegment = (MoveSegmentEndIndex >= LastSegmentEndIndex);
		//const bool bLastPathChunk = (MetaNavPath == nullptr || MetaNavPath->IsLastSection());

		if (bCollidedWithGoal)
		{
			// check if collided with goal actor
			OnSegmentFinished();
			OnPathFinished(EPathFollowingResult::Success, FPathFollowingResultFlags::None);
		}
		else if (MoveSegmentEndIndex > PreciseAcceptanceRadiusCheckStartNodeIndex && HasReachedDestination(CurrentLocation))
		{
			OnSegmentFinished();
			OnPathFinished(EPathFollowingResult::Success, FPathFollowingResultFlags::None);
		}
		else if (bFollowingLastSegment && bMoveToGoalOnLastSegment)
		{
			// use goal actor for end of last path segment
			// UNLESS it's partial path (can't reach goal)
			if (DestinationActor.IsValid() && Path->IsPartial() == false)
			{
				const FVector AgentLocation = DestinationAgent ? DestinationAgent->GetNavAgentLocation() : DestinationActor->GetActorLocation();
				// note that the condition below requires GoalLocation to be in world space.
				const FVector GoalLocation = FQuatRotationTranslationMatrix(DestinationActor->GetActorQuat(), AgentLocation).TransformPosition(MoveOffset);

				CurrentDestination.Set(NULL, GoalLocation);

				//UE_VLOG(this, LogPathFollowing, Log, TEXT("Moving directly to move goal rather than following last path segment"));
				//UE_VLOG_LOCATION(this, LogPathFollowing, VeryVerbose, GoalLocation, 30, FColor::Green, TEXT("Last-segment-to-actor"));
				//UE_VLOG_SEGMENT(this, LogPathFollowing, VeryVerbose, CurrentLocation, GoalLocation, FColor::Green, TEXT_EMPTY);
			}

			UpdateMoveFocus();

#if !UE_BUILD_SHIPPING
			DEBUG_bMovingDirectlyToGoal = true;
#endif // !UE_BUILD_SHIPPING
		}
		// check if current move segment is finished
		else if (HasReachedCurrentTarget(CurrentLocation))
		{
			OnSegmentFinished();
			SetNextMoveSegment();
		}
	}

	if (bCanUpdateState && Status == EPathFollowingStatus::Moving && MoveRequestId == GetCurrentRequestId())
	{
		// gather location samples to detect if moving agent is blocked
		const bool bHasNewSample = UpdateBlockDetection();
		if (bHasNewSample && IsBlocked())
		{
			OnPathFinished(EPathFollowingResult::Blocked, FPathFollowingResultFlags::None);
		}
	}
}

void UFlockPathFollowingComponent::FollowPathSegment(float DeltaTime)
{
	if (!Path.IsValid() || MovementComp == nullptr)
	{
		return;
	}

	const FVector CurrentLocation = MovementComp->GetActorFeetLocation();
	const FVector CurrentTarget = GetCurrentTargetLocation();

	// set to false by default, we will set set this back to true if appropriate
	bIsDecelerating = false;

	FVectorFieldPath* fieldpath = Path->CastPath<FVectorFieldPath>();
	AGameGrid * grid = Cast<AGameGrid>(fieldpath->GetNavigationDataUsed());
	UFlowFieldSolutionLayer* solution = grid->GetLayerByIndex<UFlowFieldSolutionLayer>(fieldpath->GetSolutionID());
	UGridTile* currenttile = grid->GetTileFromLocation(CurrentLocation);
	FVector outvelocityvector;

	if (solution->GetFlowVectorForTile(currenttile, outvelocityvector))
	{
		PostProcessMove.ExecuteIfBound(this, outvelocityvector);
		MovementComp->RequestDirectMove(outvelocityvector, true);
	}
	else if (solution->IsGoalTile(currenttile))
	{
		bCollidedWithGoal = true;
	}
}
