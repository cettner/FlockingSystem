// Fill out your copyright notice in the Description page of Project Settings.
#include "FlockAIController.h"
#include "../Navigation/FlockPathFollowingComponent.h"


AFlockAIController::AFlockAIController()
{
	SetPathFollowingComponent(CreateDefaultSubobject<UFlockPathFollowingComponent>(TEXT("PathFollowingComp")));
}
