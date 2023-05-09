// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/PathFollowingComponent.h"

#include "Interfaces/FlockAgentInterface.h"
#include "FlockPathFollowingComponent.generated.h"

/**
 * 
 */
UCLASS()
class UFlockPathFollowingComponent : public UPathFollowingComponent, public IFlockAgentInterface
{
	GENERATED_BODY()

	protected:
		FVector GetFlockAgentLocation() const override;
		FVector GetFlockAgentDirection() const override;

	protected:
		/** notify about finishing move along current path segment */
		virtual void OnSegmentFinished() override;
		void OnPathFinished(const FPathFollowingResult& Result) override;
		virtual void UpdatePathSegment() override;
		/** follow current path segment */
		virtual void FollowPathSegment(float DeltaTime) override;
		virtual FAIRequestID RequestMove(const FAIMoveRequest& RequestData, FNavPathSharedPtr InPath) override;
	
};
