// Fill out your copyright notice in the Description page of Project Settings.

#include "CanvasHelpers/ReplicatedCanvasManager.h"

#include "CanvasManagerSettings.h"
#include "CanvasHelpers/WorldCanvasSubsystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"



AReplicatedCanvasManager::AReplicatedCanvasManager() {
	PrimaryActorTick.bCanEverTick = true;
	bNetLoadOnClient = true;
	bReplicates = true;
	bAlwaysRelevant = true;

	static ConstructorHelpers::FObjectFinder<UCanvasManagerSettings> Finder(TEXT("/ReplicatedDrawingWidgetPlugin/CanvasManagerSettings.CanvasManagerSettings"));
	if (Finder.Succeeded())	{
		MaxNumberOfLines = Finder.Object->MaxNumberOfLines;
		MaxLineDuration = Finder.Object->MaxLineDuration;
		
	}
	else {
		MaxNumberOfLines = 0;
		MaxLineDuration = 0;
	}
}

void AReplicatedCanvasManager::BeginPlay() {
	Super::BeginPlay();
	/* The subsystem spawned us on the server, but let's tell it who we are on all clients, since it isn't replicated. */
	if (const auto subsys = GetWorld()->GetSubsystem<UWorldCanvasSubsystem>()) {
		subsys->CanvasManager = this;
	}
}

void AReplicatedCanvasManager::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (HasAuthority()) {
		CleanLines();
	}
}

void AReplicatedCanvasManager::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AReplicatedCanvasManager, Boards);
	
}

void AReplicatedCanvasManager::ReplicateLineAdded_Implementation(FName boardname, FCanvasLineData newLine) {
	OnLineAdded.Broadcast(boardname, newLine);
}

void AReplicatedCanvasManager::CleanLines() {
	if (MaxNumberOfLines <= 0 && MaxLineDuration <= 0) {
		return;
	}
	
	for (FReplicatedCanvasData& board : Boards) {
		while (board.Lines.Num() > 0 && (board.Lines.Num() > MaxNumberOfLines || board.Lines[0].GetLineLifetime(GetWorld()) > MaxLineDuration)) {
			board.Lines.RemoveAt(0);
		}	
	
	}	
}

TArray<FCanvasLineData> AReplicatedCanvasManager::GetCanvasLines(FName canvas) const {
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
