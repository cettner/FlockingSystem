// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Navigation/Interfaces/FlockAgentGoalInterface.h"

#include "TestGoalActor.generated.h"

UCLASS()
class FLOCKINGSYSTEM_API ATestGoalActor : public AActor, public IFlockAgentGoalInterface 
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestGoalActor();

public:
	virtual FVector GetGoalRootLocation() const override;
	virtual FBoxSphereBounds GetGoalBounds() const override;


protected:
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* MeshComp = nullptr;

};
