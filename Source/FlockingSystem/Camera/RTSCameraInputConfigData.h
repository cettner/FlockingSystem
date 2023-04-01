// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnhancedInput/Public/InputAction.h"

#include "RTSCameraInputConfigData.generated.h"

/**
 * 
 */
UCLASS()
class FLOCKINGSYSTEM_API URTSCameraInputConfigData : public UDataAsset
{
	GENERATED_BODY()
	


public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UInputAction* IA_Move = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UInputAction* IA_Zoom = nullptr;

};
