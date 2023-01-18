// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FlockAgentInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFlockAgentInterface : public UInterface
{
	GENERATED_BODY()
};

/*Forward Declaration*/
class UFlock;

class  IFlockAgentInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	void SetFlock(UFlock* InFlock) { CurrentFlock = InFlock; };

	virtual FVector GetFlockAgentLocation() const;

	virtual FVector GetFlockAgentDirection() const;

	inline UFlock* GetFlock() const { return CurrentFlock; }

public:
	virtual FVector CalcSeparation();
	virtual FVector CalcAlignment();
	virtual FVector CalcCohesion();


/*Runtime*/
protected:
	UPROPERTY()
	UFlock* CurrentFlock = nullptr;

};
