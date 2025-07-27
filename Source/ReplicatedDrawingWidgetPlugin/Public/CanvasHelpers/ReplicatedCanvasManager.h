// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/GameFramework/Info.h"
#include "CanvasHelpers/ReplicatedCanvasData.h"
#include "ReplicatedCanvasManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLineAddedSignature, FName, board, const FCanvasLineData&, line);

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

UCLASS()
class REPLICATEDDRAWINGWIDGETPLUGIN_API AReplicatedCanvasManager : public AInfo {
	GENERATED_BODY()
	UPROPERTY(Replicated)
	TArray<FReplicatedCanvasData> Boards;

public:
	// Sets default values for this actor's properties
	AReplicatedCanvasManager();

	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION(NetMulticast, reliable)
	void ReplicateLineAdded(FName boardname, FCanvasLineData newLine);
	

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	TArray<FCanvasLineData> GetCanvasLines(FName canvas);

	void AddLine(FName boardname, FCanvasLineData newLine);

	void CleanBoard(FName boardname);

	UFUNCTION(Server, Reliable)
	void AddLineOnServer(FName boardname, FCanvasLineData newLine);
	
	UPROPERTY(BlueprintAssignable)
	FOnLineAddedSignature OnLineAdded;
	
};
