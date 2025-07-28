// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CanvasManagerSettings.generated.h"

/**
 * 
 */
UCLASS(NotBlueprintType, NotBlueprintable)
class REPLICATEDDRAWINGWIDGETPLUGIN_API UCanvasManagerSettings : public UDataAsset {
	GENERATED_BODY()

public:
	/* If a board exceeds this number of lines, the oldest will be removed from the manager. Boards will still keep this line. */
	UPROPERTY(EditAnywhere, Category = "Canvas Settings")
	int MaxNumberOfLines = 0;
	/* If a line's duration (in seconds) on a board exceeds this, it will be removed from the manager. Boards will still keep this line. */
	UPROPERTY(EditAnywhere, Category = "Canvas Settings")
	float MaxLineDuration = 0.f;
};
