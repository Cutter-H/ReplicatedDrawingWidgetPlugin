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
	
	/* Bound to the world and called when each actor spawns. Used to add the player state helpers. */
	UFUNCTION()
	void OnActorSpawned(AActor* SpawnedActor);
public:
	/* We bind the above function here. */
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	/* Calls this same function on the Canvas Manager. */
	UFUNCTION()
	TArray<FCanvasLineData> GetBoardData(FName board);

	/* We spawn the Canvas Manager here. We won't need to assign it here because it'll do that on its BeginPlay.
	 * The alternative is we assign it here, but we have to do a check in its BeginPlay. So we just avoid it. 
	 */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/* Replicator actor used to store drawing data. */
	UPROPERTY(BlueprintReadOnly, Category="Canvas")
	TObjectPtr<AReplicatedCanvasManager> CanvasManager;
	
};
