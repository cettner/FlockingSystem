// Fill out your copyright notice in the Description page of Project Settings.


#include "FlockPathFollowingComponent.h"
#include "../GridSystem/GameGrid.h"

#include "AIController.h"

void UFlockPathFollowingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (Status == EPathFollowingStatus::Moving)
	{
		FVector desiredflowfieldirection;
		if (GetCurrentFlowFieldVector(desiredflowfieldirection))
		{
			UpdateObstacles(desiredflowfieldirection);
			UpdateFlock(desiredflowfieldirection);
		}

		if (IsUsingSteering())
		{
			UpdateMovementTiles();
		}
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

UFlowFieldSolutionLayer* UFlockPathFollowingComponent::GetFlowFieldSolution() const
{
	UFlowFieldSolutionLayer* retval = nullptr;
	if (Path.IsValid())
	{
		const FVectorFieldPath* fieldpath = Path->CastPath<FVectorFieldPath>();
		const AGameGrid* grid = CastChecked<AGameGrid>(fieldpath->GetNavigationDataUsed());
		retval = grid->GetLayerByIndex<UFlowFieldSolutionLayer>(fieldpath->GetSolutionID());
	}

	return retval;
}

bool UFlockPathFollowingComponent::GetCurrentFlowFieldVector(FVector& OutVector)
{
	bool success = false;
	const UFlowFieldSolutionLayer* solution = GetFlowFieldSolution();
	if (IsValid(solution))
	{
		const AGameGrid* grid = solution->GetGameGrid();
		const FVector currentlocation = MovementComp->GetActorFeetLocation();
		const UGridTile* currenttile = grid->GetTileFromLocation(currentlocation);

		if (currenttile != nullptr)
		{
			success = solution->GetFlowVectorForTile(currenttile, OutVector);
		}
	}

	return success;
}

void UFlockPathFollowingComponent::SetSteeringTile(const UGridTile* InTile)
{
	SteeringTile = InTile;
	if (InTile != nullptr)
	{
		AGameGrid* grid = InTile->GetGameGrid();
		grid->SetTileColor(InTile->GetTileID(), FLinearColor::Blue);
		grid->SetTileVisible(InTile->GetTileID(), true);
	}

}

bool UFlockPathFollowingComponent::ShouldEnableSteering() const
{
	bool retval = false;
	if (SteeringTile != nullptr)
	{
		retval = true;
	}
	else if (ScannedObstacles.bisBlocked)
	{
		retval = true;
	}
	return retval;
}

bool UFlockPathFollowingComponent::IsAngleInRange(float StartAngle, float EndAngle, float Angle) const
{
	// Ensure that AngleA and AngleB are normalized to the range [0, 360)
	StartAngle = FMath::Fmod(StartAngle, 360.0f);
	EndAngle = FMath::Fmod(EndAngle, 360.0f);

	// Normalize the AngleToCheck to the range [0, 360)
	Angle = FMath::Fmod(Angle, 360.0f);

	if (StartAngle < EndAngle)
	{
		// Case where the range does not wrap around 360 degrees
		if (Angle >= StartAngle && Angle <= EndAngle)
		{
			return true;
		}
	}
	else if (StartAngle > EndAngle)
	{
		// Case where the range wraps around 360 degrees
		if (Angle >= StartAngle || Angle <= EndAngle)
		{
			return true;
		}
	}
	else
	{
		// Case where AngleA and AngleB are the same
		if (Angle == StartAngle)
		{
			return true;
		}
	}

	return false;
}

void UFlockPathFollowingComponent::UpdateMovementTiles()
{
	if (Path.IsValid())
	{
		const FVectorFieldPath* fieldpath = Path->CastPath<FVectorFieldPath>();
		const AGameGrid* grid = CastChecked<AGameGrid>(fieldpath->GetNavigationDataUsed());

		if (CurrentTile == nullptr)
		{
			CurrentTile = grid->GetTileFromLocation(GetFlockAgentLocation());
		}
		else
		{
			const UGridTile* currenttile = grid->GetTileFromLocation(GetFlockAgentLocation());

			if (CurrentTile == SteeringTile)
			{
				SetSteeringTile(nullptr);
			}

			if (CurrentTile != currenttile)
			{
				PreviousTile = CurrentTile;
				CurrentTile = currenttile;
			}

		}
	}

}

void UFlockPathFollowingComponent::UpdateObstacles(const FVector DesiredDirection)
{
	ScannedObstacles.Reset();
	const FObstacleTraceData obstracedata = GetObstacleTraceData();
	const FVector agentlocation = GetFlockAgentLocation();
	const float agentradius = GetFlockAgentRadius();


	int32 halftracecount = obstracedata.TraceCount / 2;
	if (obstracedata.TraceCount % 2 == 0)
	{
		// Even number of traces, adjust half count
		halftracecount++;
	}
	const float MinOffset = -agentradius - (agentradius *.2);
	const float MaxOffset = agentradius + (agentradius * .2);

	for (int32 i = 0; i < obstracedata.TraceCount; i++)
	{
		const float Offset = FMath::Lerp(MinOffset, MaxOffset, static_cast<float>(i) / (obstracedata.TraceCount - 1));
		const FVector OffsetVector = FVector::CrossProduct(DesiredDirection, FVector::UpVector).GetSafeNormal() * Offset;
		const FVector TraceStart = agentlocation + OffsetVector;
		const FVector TraceEnd = TraceStart + DesiredDirection * obstracedata.TraceLength;

		// Perform the line trace and handle the results
		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.TraceTag = "DebugMovement";
		GetWorld()->DebugDrawTraceTag = CollisionParams.TraceTag;
		CollisionParams.AddIgnoredActor(GetOwner<AAIController>()->GetPawn()); // Ignore the owner of this component

		if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, obstracedata.TraceChannel, CollisionParams))
		{
			if (HitResult.IsValidBlockingHit())
			{
				const FVector collisionvector = HitResult.Location - agentlocation;
				ScannedObstacles.bisBlocked = true;
				ScannedObstacles.collisionscore += collisionvector;
				ScannedObstacles.collisionscore.Normalize();
			}
		}
	}

	const bool shouldusesteering = ShouldEnableSteering();
	SetUseSteering(shouldusesteering);
}

void UFlockPathFollowingComponent::UpdateFlock(const FVector DesiredDirection)
{

}

void UFlockPathFollowingComponent::SetUseSteering(const bool InUseSteering)
{
	if (!InUseSteering && bIsUsingSteering)
	{
		CurrentTile = nullptr;
		PreviousTile = nullptr;
		SetSteeringTile(nullptr);
	}
	bIsUsingSteering = InUseSteering;
}

void UFlockPathFollowingComponent::ApplyObstacleSteering(FVector& OutBaseVector, const UFlowFieldSolutionLayer* InSolution)
{

	if (SteeringTile != nullptr)
	{
		FVector steerdirection = (SteeringTile->GetTileCenter() - CurrentTile->GetTileCenter()).GetSafeNormal();
		OutBaseVector = steerdirection;
	}
	// Check if there are any scanned obstacles
	else if (ScannedObstacles.bisBlocked)
	{
		FVector SteerDirection = FVector::ZeroVector;


		TArray<const UGridTile*> alternatetiles = GetAlternateTileMoves(OutBaseVector, InSolution);
		if (alternatetiles.Num())
		{
			const UGridTile * chosentile = ChooseBestSteeringTile(alternatetiles, OutBaseVector);
			SetSteeringTile(chosentile);
			FVector steerdirection = (SteeringTile->GetTileCenter() - CurrentTile->GetTileCenter()).GetSafeNormal();
			OutBaseVector = steerdirection;
		}
	}
}

TArray<const UGridTile*> UFlockPathFollowingComponent::GetAlternateTileMoves(const FVector& IntendedDirection, const UFlowFieldSolutionLayer* InSolution) const
{
	TArray<const UGridTile*> retval = TArray<const UGridTile*>();
	const TArray<FGridTileNeighbor> neighboringtiles = CurrentTile->GetNeighbors();

	for (int i = 0; i < neighboringtiles.Num(); i++)
	{
		const UGridTile* neighbortile = neighboringtiles[i].NeighborTile;

		if (neighbortile != PreviousTile)
		{
			FVector neighborflow;
			FVector neighborcenter = neighbortile->GetTileCenter();
			FVector toneighbor = (neighborcenter - CurrentTile->GetTileCenter()).GetSafeNormal();

				const float directiondotresult = FVector::DotProduct(IntendedDirection, toneighbor);

			if (directiondotresult >= 0.0f && !IntendedDirection.Equals(toneighbor, 0.0001f))
			{
				retval.Add(neighbortile);
			}

		}
	}

	return retval;
}

const UGridTile* UFlockPathFollowingComponent::ChooseBestSteeringTile(TArray<const UGridTile*>& InTiles, FVector& InDesiredDirection) const
{
	const UGridTile* BestTile = nullptr;
	double BestScore = -FLT_MAX;

	for (const UGridTile* Tile : InTiles)
	{
		// Calculate a score for the tile based on your desired criteria, including the DesiredDirection
		double Score = CalculateSteeringTileScore(Tile, InDesiredDirection);

		// Update the best tile if the current tile has a higher score
		if (Score > BestScore)
		{
			BestTile = Tile;
			BestScore = Score;
		}
	}

	return BestTile;
}

const float UFlockPathFollowingComponent::CalculateSteeringTileScore(const UGridTile* Tile, const FVector& DesiredDirection) const
{
	const FVector TileDirection = (Tile->GetTileCenter() - CurrentTile->GetTileCenter()).GetSafeNormal();
	FVector TileFlowVector;

	// Calculate the dot product of the tile flow field vector and desired direction
	const float FlowDotProduct = 0.0f;
	
	if (GetFlowFieldSolution()->GetFlowVectorForTile(Tile, TileFlowVector))
	{
		FVector::DotProduct(TileFlowVector, DesiredDirection);
	}

	// Calculate the dot product of the tile direction and desired direction
	const float DirectionDotProduct = FVector::DotProduct(TileDirection, DesiredDirection);

	const float CollisionScoreDotProduct = FVector::DotProduct(TileDirection, ScannedObstacles.collisionscore);

	float PreviousMotionDotProduct = 0.0f;

	if (PreviousTile != nullptr)
	{
		const FVector PreviousDirection = (CurrentTile->GetTileCenter() - PreviousTile->GetTileCenter()).GetSafeNormal();
		PreviousMotionDotProduct = FVector::DotProduct(TileDirection, PreviousDirection);
	}

	// Return the sum of the dot products as the tile score
	float retval = DirectionDotProduct + FlowDotProduct + PreviousMotionDotProduct - (CollisionScoreDotProduct * .7);
	return retval;
}

void UFlockPathFollowingComponent::ApplyFlockSteering(FVector& InBaseVector)
{
}

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

bool UFlockPathFollowingComponent::IsFlockMember(const AActor* InActor) const
{
	bool retval = false;
	if (Path.IsValid())
	{
		if(const APawn* pawn = Cast<APawn>(InActor))
		{
			const AAIController* aic = pawn->GetController<AAIController>();
			if (IsValid(aic))
			{
				const FVectorFieldPath* fieldpath = Path->CastPath<FVectorFieldPath>();
				const AGameGrid* grid = Cast<AGameGrid>(fieldpath->GetNavigationDataUsed());
				const UFlowFieldSolutionLayer* solution = grid->GetLayerByIndex<UFlowFieldSolutionLayer>(fieldpath->GetSolutionID());
				if (IsValid(solution))
				{
					retval = solution->IsAgentSubscribed(aic->GetPathFollowingComponent());
				}
			}
		}
	}
	return retval;
}

const TSet<const UObject*> UFlockPathFollowingComponent::GetFlockAgents() const
{
	return TSet<const UObject*>();
}

float UFlockPathFollowingComponent::GetFlockAgentRadius() const
{
	float retval = 0.0f;
	APawn* pawn = GetOwner<AAIController>()->GetPawn();

	if (pawn)
	{
		retval = pawn->GetSimpleCollisionRadius();
	}

	return retval;
}

void UFlockPathFollowingComponent::OnPathFinished(const FPathFollowingResult& Result)
{
	if (Path.IsValid())
	{
		FVectorFieldPath* fieldpath = Path->CastPath<FVectorFieldPath>();
		AGameGrid* grid = Cast<AGameGrid>(fieldpath->GetNavigationDataUsed());
		UFlowFieldSolutionLayer* solution = grid->GetLayerByIndex<UFlowFieldSolutionLayer>(fieldpath->GetSolutionID());
		solution->UnSubscribeAgent(this);
	}

	Super::OnPathFinished(Result);

}

void UFlockPathFollowingComponent::UpdatePathSegment()
{
#if !UE_BUILD_SHIPPING
	DEBUG_bMovingDirectlyToGoal = false;
#endif // !UE_BUILD_SHIPPING

	if ((Path.IsValid() == false) || (MovementComp == nullptr))
	{
		//UE_CVLOG(Path.IsValid() == false, this, LogPathFollowing, Log, TEXT("Aborting move due to not having a valid path object"));
		UPathFollowingComponent::OnPathFinished(EPathFollowingResult::Aborted, FPathFollowingResultFlags::InvalidPath);
		return;
	}

	const FVector CurrentLocation = MovementComp->GetActorFeetLocation();
	const bool bCanUpdateState = HasMovementAuthority();

	if (!Path->IsValid())
	{
		if (!Path->IsWaitingForRepath())
		{
			//UE_VLOG(this, LogPathFollowing, Log, TEXT("Aborting move due to path being invalid and not waiting for repath"));
			UPathFollowingComponent::OnPathFinished(EPathFollowingResult::Aborted, FPathFollowingResultFlags::InvalidPath);
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
			UPathFollowingComponent::OnPathFinished(EPathFollowingResult::Success, FPathFollowingResultFlags::None);
		}
		else if (MoveSegmentEndIndex > PreciseAcceptanceRadiusCheckStartNodeIndex && HasReachedDestination(CurrentLocation))
		{
			OnSegmentFinished();
			UPathFollowingComponent::OnPathFinished(EPathFollowingResult::Success, FPathFollowingResultFlags::None);
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
			UPathFollowingComponent::OnPathFinished(EPathFollowingResult::Blocked, FPathFollowingResultFlags::None);
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

	const UFlowFieldSolutionLayer* solution = GetFlowFieldSolution();

	if (IsValid(solution))
	{
		const FVectorFieldPath* fieldpath = Path->CastPath<FVectorFieldPath>();
		const AGameGrid* grid = CastChecked<AGameGrid>(fieldpath->GetNavigationDataUsed());
		const UGridTile* currenttile = grid->GetTileFromLocation(CurrentLocation);

		FVector outvelocityvector;

		if (solution->GetFlowVectorForTile(currenttile, outvelocityvector))
		{
			ApplyObstacleSteering(outvelocityvector, solution);
			PostProcessMove.ExecuteIfBound(this, outvelocityvector);
			MovementComp->RequestDirectMove(outvelocityvector, true);
		}
		else if (solution->IsGoalTile(currenttile))
		{
			bCollidedWithGoal = true;
		}
	}
	else
	{
		Path.Reset();
	}
}

FAIRequestID UFlockPathFollowingComponent::RequestMove(const FAIMoveRequest& RequestData, FNavPathSharedPtr InPath)
{
	FAIRequestID retval = FAIRequestID();

	FVectorFieldPath* fieldpath = InPath->CastPath<FVectorFieldPath>();
	AGameGrid* grid = Cast<AGameGrid>(fieldpath->GetNavigationDataUsed());
	const int32 layerid = fieldpath->GetSolutionID();
	UFlowFieldSolutionLayer* solution = grid->GetLayerByIndex<UFlowFieldSolutionLayer>(layerid);

	if (IsValid(solution))
	{
		solution->SubscribeAgent(this);
		retval = Super::RequestMove(RequestData, InPath);

	}
	else
	{
		retval = FAIRequestID::InvalidRequest;
	}

	return retval;
}

void UFlockPathFollowingComponent::Reset()
{
	Super::Reset();
	CurrentTile = nullptr;
	PreviousTile = nullptr;
	SteeringTile = nullptr;

	PreviousObstacleSteeringForce = FVector::ZeroVector;
	ScannedObstacles.Reset();
	bIsUsingSteering = false;
	
}
