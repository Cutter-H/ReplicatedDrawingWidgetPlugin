// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ReplicatedCanvasData.h"
#include "Components/ActorComponent.h"
#include "CanvasPlayerStateHelper.generated.h"

class AReplicatedCanvasManager;

UCLASS(NotBlueprintable, NotBlueprintType)
class REPLICATEDDRAWINGWIDGETPLUGIN_API UCanvasPlayerStateHelper : public UActorComponent{
	GENERATED_BODY()
	
public:
	UCanvasPlayerStateHelper();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	/* Hard ref to the canvas manager so we don't have to find it or go through the subsystem every time. */ 
	UPROPERTY(BlueprintReadOnly, Replicated)
	TObjectPtr<AReplicatedCanvasManager> CanvasManager;

	/* Adds a new line to the server. */
	UFUNCTION(Server, Reliable)
	void AddLine(FName boardname, FCanvasLineData newLine);

	/* Cleans the board on the server. DOES NOT IMMEDIATELY ERASE IT ON ALL CLIENTS! They will need to refresh is from the widget. */
	UFUNCTION(Server, Reliable)
	void CleanBoard(FName boardname);
};
