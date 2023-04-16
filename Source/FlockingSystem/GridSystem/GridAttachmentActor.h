// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GameGrid.h"
#include "GridLayerComponent.h"
#include "GridAttachmentActor.generated.h"

UCLASS()
class FLOCKINGSYSTEM_API AGridAttachmentActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridAttachmentActor();

	virtual void SetRootTile(UGridTile* InRootTile);
	AGameGrid* GetGameGrid() const;
	FORCEINLINE UGridTile * GetRootTile() const{ return RootTile; }

protected:
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;

protected: 
	AGameGrid* FindGrid(UWorld * InWorld);


protected:
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* DebugMeshComp = nullptr;

	UPROPERTY(EditAnywhere)
	UGridLayerComponent* GridLayerComp = nullptr;

protected:
	UGridTile * RootTile = nullptr;

#ifdef WITH_EDITOR
protected:
	virtual void PostRootTileChanged();

protected:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditMove(bool bFinished) override;
#endif
};
