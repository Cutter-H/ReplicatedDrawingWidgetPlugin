// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ReplicatedCanvasData.generated.h"

class USlateBrushAsset;
/**
 * 
 */
USTRUCT(BlueprintType)
struct FCanvasPenData {
	GENERATED_BODY()

	FCanvasPenData() {}
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pen")
	TObjectPtr<USlateBrushAsset> Brush = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pen")
	FColor Tint = FColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pen")
	float Size = 1.f;
	
};

USTRUCT(BlueprintType)
struct FCanvasLineData {
	GENERATED_BODY()

	FCanvasLineData(){}
	FCanvasLineData(FName name, float time, FVector2D startP, FVector2D startD, FVector2D stopP,  FVector2D stopD, float size = 1.f, FColor color = FColor::White) :
	DrawingPlayer(name), DrawnTime(time), StartingPoint(startP), StartingDirection(startD), StoppingPoint(stopP), StoppingDirection(stopD), Size(size), Tint(color)
	{};

	UPROPERTY(BlueprintReadOnly, Category = "Line")
	FName DrawingPlayer = FName();
	UPROPERTY(BlueprintReadOnly, Category = "Line")
	float DrawnTime = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "Line")
	FVector2D StartingPoint = FVector2D::ZeroVector;
	UPROPERTY(BlueprintReadOnly, Category = "Line")
	FVector2D StartingDirection = FVector2D::ZeroVector;
	UPROPERTY(BlueprintReadOnly, Category = "Line")
	FVector2D StoppingPoint = FVector2D::ZeroVector;
	UPROPERTY(BlueprintReadOnly, Category = "Line")
	FVector2D StoppingDirection = FVector2D::ZeroVector;
	UPROPERTY(BlueprintReadOnly, Category = "Line")
	float Size = 1.f;
	UPROPERTY(BlueprintReadOnly, Category = "Line")
	FColor Tint = FColor::White;

	float GetLineLife(UObject* worldContext) const;
	
};
