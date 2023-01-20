// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Navigation/FlockNavManager.h"
#include "FlockingSystemGameMode.generated.h"

UCLASS(minimalapi)
class AFlockingSystemGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFlockingSystemGameMode();
	UFlockNavManager* GetFlockManager() const;

protected:
	virtual void PostInitializeComponents() override;



protected:
	UPROPERTY(transient)
	UFlockNavManager* FlockManager = nullptr;
};



