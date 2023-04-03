// Fill out your copyright notice in the Description page of Project Settings.


#include "GridAttachmentActor.h"

#include "EngineUtils.h"

// Sets default values
AGridAttachmentActor::AGridAttachmentActor()
{
	PrimaryActorTick.bCanEverTick = false;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    DebugMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DebugMeshComp"));
    DebugMeshComp->SetupAttachment(RootComponent);
}

void AGridAttachmentActor::SetRootTile(UGridTile* InRootTile)
{
    RootTile = InRootTile;

    if (IsValid(RootTile))
    {
        const FVector tilelocation = InRootTile->GetTileCenter();
        SetActorLocation(tilelocation);
    }
}

AGameGrid * AGridAttachmentActor::GetGameGrid() const
{
    AGameGrid* retval = nullptr;
    if (IsValid(RootTile))
    {
        retval = RootTile->GetGameGrid();
    }
    return retval;
}

void AGridAttachmentActor::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    AGameGrid* foundgrid = FindGrid(GetWorld());

    if (IsValid(foundgrid))
    {
        UGridTile* mytile = foundgrid->GetTileFromLocation(GetActorLocation());

        if (IsValid(mytile))
        {
            SetRootTile(mytile);
        }
    }
}

AGameGrid* AGridAttachmentActor::FindGrid(UWorld* InWorld)
{
    AGameGrid* retval = nullptr;
    if (IsValid(InWorld))
    {
        for (TActorIterator<AGameGrid> ActorItr(InWorld); ActorItr; ++ActorItr)
        {
            AGameGrid* found = *ActorItr;

            if (found->IsValidGridLocation(GetActorLocation()))
            {
                retval = found;
                break;
            }
        }
    }

    return retval;
}

#ifdef WITH_EDITOR

static FName Name_RelativeLocation = USceneComponent::GetRelativeLocationPropertyName();
static FName Name_RelativeRotation = USceneComponent::GetRelativeRotationPropertyName();
static FName Name_RelativeScale3D = USceneComponent::GetRelativeScale3DPropertyName();

void AGridAttachmentActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    FProperty* MemberPropertyThatChanged = PropertyChangedEvent.MemberProperty;
    const FName MemberPropertyName = MemberPropertyThatChanged != NULL ? MemberPropertyThatChanged->GetFName() : NAME_None;

    const bool bTransformationChanged = (MemberPropertyName == Name_RelativeLocation || MemberPropertyName == Name_RelativeRotation || MemberPropertyName == Name_RelativeScale3D);

    if (bTransformationChanged)
    {
        AGameGrid* foundgrid = FindGrid(GetWorld());

        if (IsValid(foundgrid))
        {
            UGridTile* mytile = foundgrid->GetTileFromLocation(GetActorLocation());

            if (IsValid(mytile))
            {
                SetRootTile(mytile);
            }
        }
    }

}

void AGridAttachmentActor::PostEditMove(bool bFinished)
{
    Super::PostEditMove(bFinished);

    if (bFinished == true)
    {
        AGameGrid* foundgrid = FindGrid(GetWorld());

        if (IsValid(foundgrid))
        {
            UGridTile* mytile = foundgrid->GetTileFromLocation(GetActorLocation());

            if (IsValid(mytile))
            {
                SetRootTile(mytile);
            }
        }
    }
}
#endif

