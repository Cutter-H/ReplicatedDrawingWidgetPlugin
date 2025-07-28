// Fill out your copyright notice in the Description page of Project Settings.


#include "CanvasHelpers/CanvasPlayerStateHelper.h"

#include "CanvasHelpers/ReplicatedCanvasManager.h"
#include "CanvasHelpers/WorldCanvasSubsystem.h"
#include "Net/UnrealNetwork.h"

#include "Engine/World.h"


UCanvasPlayerStateHelper::UCanvasPlayerStateHelper() {
	SetIsReplicatedByDefault(true);
	SetComponentTickEnabled(false);
	SetAutoActivate(true);
}

void UCanvasPlayerStateHelper::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCanvasPlayerStateHelper, CanvasManager);
}

void UCanvasPlayerStateHelper::CleanBoard_Implementation(FName boardname) {
	if (!CanvasManager) {
		if (const auto subsys = GetWorld()->GetSubsystem<UWorldCanvasSubsystem>()) {
			CanvasManager = subsys->CanvasManager;
		}
	}
		
	if (CanvasManager) {
		CanvasManager->CleanBoard(boardname);	
	}
}

void UCanvasPlayerStateHelper::AddLine_Implementation(FName boardname, FCanvasLineData newLine) {
	if (!CanvasManager) {
		if (const auto subsys = GetWorld()->GetSubsystem<UWorldCanvasSubsystem>()) {
			CanvasManager = subsys->CanvasManager;
		}
	}
		
	if (CanvasManager) {
		CanvasManager->AddLine(boardname, newLine);	
	}
	
}
