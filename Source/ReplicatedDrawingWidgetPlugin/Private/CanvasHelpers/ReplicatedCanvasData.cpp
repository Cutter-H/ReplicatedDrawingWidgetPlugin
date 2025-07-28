// Fill out your copyright notice in the Description page of Project Settings.

#include "CanvasHelpers/ReplicatedCanvasData.h"
#include "Kismet/GameplayStatics.h"

/* Function is over here to avoid the gameplay statics include on the header.
 * The header will be included in multiple headers, so it should be as lightweight as possible. Unless I'm misunderstanding..
 */
float FCanvasLineData::GetLineLifetime(UObject* worldContext) const {
	return UGameplayStatics::GetTimeSeconds(worldContext) - DrawnTime; 
}
