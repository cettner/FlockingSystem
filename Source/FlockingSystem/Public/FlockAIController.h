// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "../GridSystem/GameGrid.h"
#include "FlockAIController.generated.h"

/**
 * 
 */
UCLASS()
class FLOCKINGSYSTEM_API AFlockAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AFlockAIController();
	virtual void OnPossess(APawn* InPawn) override;

	/*Calls Request Move*/
	virtual FPathFollowingRequestResult MoveTo(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr* OutPath = nullptr) override;

	virtual void FindFieldForMoveRequest(const FAIMoveRequest& MoveRequest, FVectorFieldQuery& Query, FNavPathSharedPtr& OutPath) const;

	virtual void SetGoalActor(AActor* InGoal);

	virtual void SetGoalLocation(FVector InLocation);

protected:

	virtual bool BuildPathfindingQuery(const FAIMoveRequest& MoveRequest, FVectorFieldQuery& OutQuery) const;


protected:
	UPROPERTY(transient)
	class UBlackboardComponent* BlackboardComp;

	UPROPERTY(transient)
	class UBehaviorTreeComponent* BehaviorComp;

};
