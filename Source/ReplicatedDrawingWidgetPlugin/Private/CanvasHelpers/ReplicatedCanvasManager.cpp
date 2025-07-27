// Fill out your copyright notice in the Description page of Project Settings.
#include "CanvasHelpers/ReplicatedCanvasManager.h"

#include "CanvasHelpers/WorldCanvasSubsystem.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AReplicatedCanvasManager::AReplicatedCanvasManager() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bNetLoadOnClient = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	
}

bool AReplicatedCanvasManager::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget,
	const FVector& SrcLocation) const {
	return Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
}

// Called when the game starts or when spawned
void AReplicatedCanvasManager::BeginPlay() {
	Super::BeginPlay();
	if (const auto subsys = GetWorld()->GetSubsystem<UWorldCanvasSubsystem>()) {
		subsys->Replicator = this;
	}
}

void AReplicatedCanvasManager::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AReplicatedCanvasManager, Boards);
	
}

void AReplicatedCanvasManager::ReplicateLineAdded_Implementation(FName boardname, FCanvasLineData newLine) {
	OnLineAdded.Broadcast(boardname, newLine);
}

// Called every frame
void AReplicatedCanvasManager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

TArray<FCanvasLineData> AReplicatedCanvasManager::GetCanvasLines(FName canvas) {
	for (const auto& board : Boards) {
		if (board.BoardName == canvas) {
			return board.Lines;
		}
	}
	return TArray<FCanvasLineData>();
}

void AReplicatedCanvasManager::AddLine(FName boardname, FCanvasLineData newLine) {
	for (FReplicatedCanvasData& board : Boards) {
		if (board.BoardName == boardname) {
			board.Lines.Add(newLine);
			ReplicateLineAdded(boardname, newLine);
			return;
		}
	}
	
	Boards.Add(FReplicatedCanvasData(boardname, newLine));
	ReplicateLineAdded(boardname, newLine);

}

void AReplicatedCanvasManager::CleanBoard(FName boardname) {
	for (int i=0; i<Boards.Num();i++) {
		if (Boards[i].BoardName == boardname) {
			Boards.RemoveAt(i);
		}
	}
}

void AReplicatedCanvasManager::AddLineOnServer_Implementation(FName boardname, FCanvasLineData newLine) {
	AddLine(boardname, newLine);
}
