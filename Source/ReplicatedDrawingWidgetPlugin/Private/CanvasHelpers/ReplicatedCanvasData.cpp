// Fill out your copyright notice in the Description page of Project Settings.

#include "CanvasHelpers/ReplicatedCanvasData.h"
#include "Kismet/GameplayStatics.h"

float FCanvasLineData::GetLineLife(UObject* worldContext) const {
	
	return UGameplayStatics::GetTimeSeconds(worldContext) - DrawnTime; 
}
