// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ReplicatedCanvasData.h"
#include "Components/ActorComponent.h"
#include "CanvasPlayerStateHelper.generated.h"

UCLASS(NotBlueprintable, NotBlueprintType)
class REPLICATEDDRAWINGWIDGETPLUGIN_API UCanvasPlayerStateHelper : public UActorComponent{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UCanvasPlayerStateHelper();
	
	UFUNCTION(Server, Reliable)
	void AddLine(FName boardname, FCanvasLineData newLine);
};
