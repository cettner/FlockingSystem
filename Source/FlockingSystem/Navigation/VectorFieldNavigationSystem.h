// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/NavigationSystemBase.h"
#include "VectorFieldNavigationSystem.generated.h"

/**
 * 
 */
UCLASS()
class FLOCKINGSYSTEM_API UVectorFieldNavigationSystem : public UNavigationSystemBase
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float DeltaSeconds) override;
	virtual void CleanUp(const FNavigationSystem::ECleanupMode Mode) override;
	virtual void Configure(const UNavigationSystemConfig& Config) override;

	/** Called when there's a need to extend current navigation system's config with information in NewConfig */
	virtual void AppendConfig(const UNavigationSystemConfig& NewConfig) override;

	virtual void OnInitializeActors() override;

	virtual bool IsNavigationBuilt(const AWorldSettings* Settings) const override;

	virtual void ApplyWorldOffset(const FVector& InOffset, bool bWorldShift) override;

	virtual void InitializeForWorld(UWorld& World, FNavigationSystemRunMode Mode) override;

};
