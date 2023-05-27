// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/PathFollowingComponent.h"

#include "../GridSystem/Layers/FlowFieldSolutionLayer.h"
#include "Interfaces/FlockAgentInterface.h"
#include "FlockPathFollowingComponent.generated.h"


USTRUCT()
struct FObstacleScan
{
	GENERATED_USTRUCT_BODY()
public:
	FVector collisionscore = FVector::ZeroVector;
	bool bisBlocked = false;
	void Reset() 
	{
		bisBlocked = false;
		collisionscore = FVector::ZeroVector;
	}

};


UCLASS()
class UFlockPathFollowingComponent : public UPathFollowingComponent, public IFlockAgentInterface
{
	GENERATED_BODY()
		
	protected:


	protected:
		FORCEINLINE UFlowFieldSolutionLayer* GetFlowFieldSolution() const;
		FORCEINLINE bool GetCurrentFlowFieldVector(FVector& OutVector);
		FORCEINLINE bool IsUsingSteering() const { return bIsUsingSteering; }
		FORCEINLINE void SetSteeringTile(const UGridTile* InTile);
		bool IsAngleInRange(float StartAngle, float EndAngle, float Angle) const;
		virtual bool ShouldEnableSteering() const;
		virtual void UpdateMovementTiles();
		virtual void UpdateObstacles(const FVector DesiredDirection);
		virtual void UpdateFlock(const FVector DesiredDirection);

		virtual void SetUseSteering(const bool InUseSteering);
		virtual void ApplyObstacleSteering(FVector& InBaseVector, const UFlowFieldSolutionLayer* InSolution);
		virtual TArray<const UGridTile*> GetAlternateTileMoves(const FVector& IntendedDirection, const UFlowFieldSolutionLayer* InSolution) const;
		virtual const UGridTile* ChooseBestSteeringTile(TArray<const UGridTile*>& InTiles, FVector& InDesiredDirection) const;
		virtual const float CalculateSteeringTileScore(const UGridTile* Tile, const FVector& DesiredDirection) const;
		virtual void ApplyFlockSteering(FVector& InBaseVector);

	protected:
		virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	protected:
		FVector GetFlockAgentLocation() const override;
		FVector GetFlockAgentDirection() const override;
		bool IsFlockMember(const AActor* InActor) const override;
		const TSet<const UObject*> GetFlockAgents() const override;
		virtual float GetFlockAgentRadius() const;

	protected:
		/** notify about finishing move along current path segment */
		void OnPathFinished(const FPathFollowingResult& Result) override;
		virtual void UpdatePathSegment() override;
		/** follow current path segment */
		virtual void FollowPathSegment(float DeltaTime) override;
		virtual FAIRequestID RequestMove(const FAIMoveRequest& RequestData, FNavPathSharedPtr InPath) override;
		virtual void Reset() override;
	


	protected:
		/*Map Correlating an obstacle and the angles it was struck from relative to the current flowfields intended direction*/
		FObstacleScan ScannedObstacles;
		FVector PreviousObstacleSteeringForce = FVector::ZeroVector;

		const UGridTile* CurrentTile = nullptr;
		const UGridTile* PreviousTile = nullptr;
		const UGridTile* SteeringTile = nullptr;
		bool bIsUsingSteering = false;


		TSet<const UObject*> FlockAgents = TSet<const UObject*>();


};
