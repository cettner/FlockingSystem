// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FunctionalTestLevelScript.h"
#include "FlockTestLevelDriver.generated.h"

/**
 * 
 */
UCLASS()
class FLOCKINGSYSTEM_API AFlockTestLevelDriver : public AFunctionalTestLevelScript
{
	GENERATED_BODY()

		virtual void BeginPlay() override;

};
