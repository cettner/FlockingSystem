// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
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

	/** Passes move request and path object to path following */
	virtual FAIRequestID RequestMove(const FAIMoveRequest& MoveRequest, FNavPathSharedPtr Path) override;

	virtual void FindPathForMoveRequest(const FAIMoveRequest& MoveRequest, FPathFindingQuery& Query, FNavPathSharedPtr& OutPath) const override;

	virtual void SetGoalActor(AActor* InGoal);

protected:
	UPROPERTY(transient)
	class UBlackboardComponent* BlackboardComp;

	UPROPERTY(transient)
	class UBehaviorTreeComponent* BehaviorComp;

};
