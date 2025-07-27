// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CanvasHelpers/ReplicatedCanvasData.h"
#include "WorldCanvasSubsystem.generated.h"

class AReplicatedCanvasManager;

UCLASS()
class REPLICATEDDRAWINGWIDGETPLUGIN_API UWorldCanvasSubsystem : public UWorldSubsystem {
	GENERATED_BODY()

	TObjectPtr<AReplicatedCanvasManager> CanvasManager;
	
	AReplicatedCanvasManager* GetCanvasManager();

	void OnActorSpawned(AActor* SpawnedActor);
public:
	UWorldCanvasSubsystem();
	TArray<FCanvasLineData> GetBoardData(FName board);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	
	virtual void Deinitialize() override;

	UPROPERTY(BlueprintReadOnly, Category="Canvas")
	TObjectPtr<AReplicatedCanvasManager> Replicator;
	
};
