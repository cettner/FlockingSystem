// Fill out your copyright notice in the Description page of Project Settings.


#include "TestGoalActor.h"

// Sets default values
ATestGoalActor::ATestGoalActor()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComp->SetupAttachment(RootComponent);
}

FVector ATestGoalActor::GetGoalRootLocation() const
{
	return GetActorLocation();
}

FBoxSphereBounds ATestGoalActor::GetGoalBounds() const
{
	return MeshComp->Bounds;
}


