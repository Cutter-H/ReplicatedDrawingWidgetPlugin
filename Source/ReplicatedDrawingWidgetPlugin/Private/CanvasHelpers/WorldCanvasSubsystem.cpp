// Fill out your copyright notice in the Description page of Project Settings.

#include "CanvasHelpers/WorldCanvasSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "CanvasHelpers/CanvasPlayerStateHelper.h"
#include "CanvasHelpers/ReplicatedCanvasManager.h"

void UWorldCanvasSubsystem::OnActorSpawned(AActor* SpawnedActor) {
	if (SpawnedActor && SpawnedActor->IsA<APlayerState>() && SpawnedActor->HasAuthority()) {
		if (SpawnedActor->FindComponentByClass<UCanvasPlayerStateHelper>()) { return; }
		
		if (UCanvasPlayerStateHelper* comp = NewObject<UCanvasPlayerStateHelper>(SpawnedActor)) {
			comp->RegisterComponent();
			if (CanvasManager) {
				comp->CanvasManager = CanvasManager;
			}
		}
	}
}

TArray<FCanvasLineData> UWorldCanvasSubsystem::GetBoardData(FName board) {
	return TArray<FCanvasLineData>();
}

void UWorldCanvasSubsystem::OnWorldBeginPlay(UWorld& InWorld) {
	if (InWorld.GetNetMode() != NM_Client) {
		InWorld.SpawnActor<AReplicatedCanvasManager>(AReplicatedCanvasManager::StaticClass());
	}
	
}

void UWorldCanvasSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
	Super::Initialize(Collection);

	GetWorld()->AddOnActorSpawnedHandler(FOnActorSpawned::FDelegate::CreateUObject(this, &UWorldCanvasSubsystem::OnActorSpawned));
}
