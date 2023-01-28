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

	virtual void SetGoalActor(AActor* InGoal);

protected:
	UPROPERTY(transient)
	class UBlackboardComponent* BlackboardComp;

	UPROPERTY(transient)
	class UBehaviorTreeComponent* BehaviorComp;

};
