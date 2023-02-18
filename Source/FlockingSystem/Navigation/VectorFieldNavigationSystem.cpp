// Fill out your copyright notice in the Description page of Project Settings.


#include "VectorFieldNavigationSystem.h"

void UVectorFieldNavigationSystem::Tick(float DeltaSeconds)
{
	int debug = 9;
}

void UVectorFieldNavigationSystem::CleanUp(const FNavigationSystem::ECleanupMode Mode)
{
}

void UVectorFieldNavigationSystem::Configure(const UNavigationSystemConfig& Config)
{
}

void UVectorFieldNavigationSystem::AppendConfig(const UNavigationSystemConfig& NewConfig)
{
}

void UVectorFieldNavigationSystem::OnInitializeActors()
{
}

bool UVectorFieldNavigationSystem::IsNavigationBuilt(const AWorldSettings* Settings) const
{
	return false;
}

void UVectorFieldNavigationSystem::ApplyWorldOffset(const FVector& InOffset, bool bWorldShift)
{
}

void UVectorFieldNavigationSystem::InitializeForWorld(UWorld& World, FNavigationSystemRunMode Mode)
{
}
