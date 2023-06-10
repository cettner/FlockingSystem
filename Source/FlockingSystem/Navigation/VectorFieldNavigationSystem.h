// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "AI/NavigationSystemBase.h"
#include "../GridSystem/GameGrid.h"
#include "VectorFieldNavigationSystem.generated.h"



struct FVectorFieldQuery : public FPathFindingQueryData
{
public:
	FVectorFieldQuery() : FPathFindingQueryData() {}
	FVectorFieldQuery(const FVectorFieldQuery& Source);
	FVectorFieldQuery(const UObject* InOwner, const AGameGrid& InNavData, const FVector& Start, const FVector& End);
	FVectorFieldQuery(const INavAgentInterface& InNavAgent, const AGameGrid& InNavData, const FVector& Start, const FVector& End);

public:
	bool IsDynamicGoal = false;
	FORCEINLINE AActor* GetGoalActor() const { TargetGoalActor; }
	FORCEINLINE void SetGoalActor(const AActor* InActor) { TargetGoalActor = InActor; }
	FORCEINLINE bool IsGoalActor() const { return TargetGoalActor != nullptr; }

protected:
	const AActor* TargetGoalActor = nullptr;

public:
	TWeakObjectPtr<const AGameGrid> NavData;
	FNavPathSharedPtr PathInstanceToFill;
	FNavAgentProperties NavAgentProperties;
};

UCLASS()
class FLOCKINGSYSTEM_API UVectorFieldNavigationSystem : public UNavigationSystemBase
{
	GENERATED_BODY()
	
public:
	virtual bool RegisterNavData(ANavigationData* InGrid);
	virtual void UnRegisterNavData(ANavigationData* InNavData);
	/** Adds NavData to registration candidates queue - NavDataRegistrationQueue*/
	virtual void RequestRegistrationDeferred(ANavigationData& NavData);
	ANavigationData* GetNavDataForProps(const FNavAgentProperties& AgentProperties, const FVector& AgentLocation, const FVector& Extent = INVALID_NAVEXTENT) const;

	template<typename T>
	T* GetNavData() const
	{
		return Cast<T>(GetNavData());
	}
	virtual FPathFindingResult FindVectorFieldPathSync(FVectorFieldQuery& InQuery);


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


protected:

	UPROPERTY(Transient)
	TArray<TObjectPtr<ANavigationData>> NavDataSet = TArray<TObjectPtr<ANavigationData>>();

	UPROPERTY(Transient)
	TArray<TObjectPtr<ANavigationData>> NavDataRegistrationQueue;

	/** List of agents types supported by this navigation system */
	UPROPERTY(config, EditAnywhere, Category = Agents)
	TArray<FNavDataConfig> SupportedAgents;

	TMap<FNavAgentProperties, TWeakObjectPtr<ANavigationData> > AgentToNavDataMap;

	FCriticalSection NavDataRegistration;
	FCriticalSection NavDataRegistrationSection;
};
