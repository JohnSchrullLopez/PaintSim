// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MaxPossiblePercentPainted.generated.h"

/**
 * 
 */
UCLASS()
class UMaxPossiblePercentPainted : public UDataAsset
{
	GENERATED_BODY()

public:
		TMap<FString, float> DefaultMaxPaintablePercents;
};
