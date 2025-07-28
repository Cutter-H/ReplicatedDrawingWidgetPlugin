// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/GameFramework/Info.h"
#include "CanvasHelpers/ReplicatedCanvasData.h"
#include "ReplicatedCanvasManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLineAddedSignature, FName, board, const FCanvasLineData&, line);

/* Struct used for a replicated pseudo-map. */
USTRUCT()
struct FReplicatedCanvasData {
	GENERATED_BODY()
	
	FReplicatedCanvasData() {}
	FReplicatedCanvasData(FName boardName, FCanvasLineData line) { BoardName = boardName; Lines.Add(line); }
	UPROPERTY()
	FName BoardName;
	UPROPERTY()
	TArray<FCanvasLineData> Lines;
};

UCLASS(NotBlueprintable, BlueprintType)
class REPLICATEDDRAWINGWIDGETPLUGIN_API AReplicatedCanvasManager : public AInfo {
	GENERATED_BODY()

	UPROPERTY(Replicated)
	TArray<FReplicatedCanvasData> Boards;

	/* These are used for culling lines. It's so we don't have a trillion lines after 10 minutes of gameplay. */
	UPROPERTY(Replicated)
	int MaxNumberOfLines;
	UPROPERTY(Replicated)
	float MaxLineDuration;

public:
	AReplicatedCanvasManager();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	/* Announce to all the clients that we added a new line. "Welcome to the family, son." -some virus guy */
	UFUNCTION(NetMulticast, reliable)
	void ReplicateLineAdded(FName boardname, FCanvasLineData newLine);

	void CleanLines();
	

public:
	/* Getter that returns all lines on a canvas. */
	UFUNCTION(BlueprintCallable, Category = "CanvasManager")
	TArray<FCanvasLineData> GetCanvasLines(FName boardName) const;
	
	void AddLine(FName boardname, FCanvasLineData newLine);

	void CleanBoard(FName boardname);
	
	UPROPERTY(BlueprintAssignable)
	FOnLineAddedSignature OnLineAdded;
	
};

