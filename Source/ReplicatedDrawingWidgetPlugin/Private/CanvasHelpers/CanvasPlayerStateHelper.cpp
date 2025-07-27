// Fill out your copyright notice in the Description page of Project Settings.


#include "CanvasHelpers/CanvasPlayerStateHelper.h"

#include "CanvasHelpers/ReplicatedCanvasManager.h"
#include "CanvasHelpers/WorldCanvasSubsystem.h"
#include "Kismet/KismetSystemLibrary.h"


UCanvasPlayerStateHelper::UCanvasPlayerStateHelper() {
	SetIsReplicatedByDefault(true);
	SetComponentTickEnabled(false);
	SetAutoActivate(true);
}

void UCanvasPlayerStateHelper::AddLine_Implementation(FName boardname, FCanvasLineData newLine) {
	if (const auto subsys = GetWorld()->GetSubsystem<UWorldCanvasSubsystem>()) {
		if (subsys->Replicator) {
			UKismetSystemLibrary::PrintString(this, "Line Added");
			subsys->Replicator->AddLine(boardname, newLine);	
		}
	}
}
