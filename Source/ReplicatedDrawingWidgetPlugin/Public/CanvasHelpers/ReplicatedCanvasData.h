// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ReplicatedCanvasData.generated.h"

class USlateBrushAsset;
class UCurveFloat;

UENUM(BlueprintType)
enum EPenDataFadeType : uint8 {
	/* No fade will be made. */
	PENFADETYPE_NoFade = 0 UMETA(DisplayName = "No Fade"),
	/* A consistent fade of 1 to 0 will be made. */
	PENFADETYPE_Linear = 1 UMETA(DisplayName = "Linear"),
	/* A fade determined by a curve is made. */
	PENFADETYPE_Curve = 2 UMETA(DisplayName = "Curve"),	
};

UENUM(BlueprintType)
enum class EPenDrawEffect : uint8 {
	/** No effect applied */
	PENEFFECT_None = 0 UMETA(DisplayName = "None"),
	/** Advanced: Draw the element with no blending */
	PENEFFECT_NoBlending = 1 << 0 UMETA(DisplayName = "No Blending"),
	/** Advanced: Blend using pre-multiplied alpha. Ignored if NoBlending is set. */
	PENEFFECT_PreMultipliedAlpha = 1 << 1 UMETA(DisplayName = "Pre-Multiplied Alpha"),
	/** Advanced: No gamma correction should be done */
	PENEFFECT_NoGamma = 1 << 2 UMETA(DisplayName = "No Gamma"),
	/** Advanced: Change the alpha value to 1 - Alpha. */
	PENEFFECT_InvertAlpha = 1 << 3 UMETA(DisplayName = "Invert Alpha"),
};
USTRUCT(BlueprintType)
struct FCanvasPenData {
	GENERATED_BODY()

	FCanvasPenData() {}

	/* Brush used to draw the line. THIS IS NOT IMPLEMENTED*/
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Pen")
	TObjectPtr<USlateBrushAsset> Brush = nullptr;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Pen")
	bool bBrushFollowsLine = false;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Pen")
	EPenDrawEffect DrawEffect = EPenDrawEffect::PENEFFECT_None;
	
	/* Distance between each stamp of the brush. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pen", meta=(EditCondition = "Brush != nullptr", EditConditionHides))
	float StepSize = 1.f;

	/* Tint of the line. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pen", meta=(EditCondition = "Brush == nullptr", EditConditionHides))
	FLinearColor Tint = FColor::White;

	/* Size of the line. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pen", meta=(EditCondition = "Brush != nullptr", EditConditionHides))
	FVector2D BrushSize = FVector2D::One();

	/* Size of the line. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pen", meta=(EditCondition = "Brush == nullptr", EditConditionHides))
	float LineSize = 1.f;
	
	/* Determines how the opacity fades if the board supports fading. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pen|Fading")
	TEnumAsByte<EPenDataFadeType> OpacityFadeType = EPenDataFadeType::PENFADETYPE_Linear;

	/* A curve that makes the line opacity decrease if the board fades lines. The value used will be an absolute value of -1 to 1. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pen|Fading", meta=(EditCondition = "OpacityFadeType == EPenDataFadeType::PENFADETYPE_Curve", EditConditionHides))
	TObjectPtr<UCurveFloat> OpacityFadeCurve = nullptr;

	/* Determines how the line shrinks if the board supports fading. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pen|Fading")
	TEnumAsByte<EPenDataFadeType> SizeFadeType = EPenDataFadeType::PENFADETYPE_Linear;;
	
	/* A curve that makes the line size decrease if the board fades lines. The value used will be an absolute value of -1 to 1. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pen|Fading", meta=(EditCondition = "SizeFadeType == EPenDataFadeType::PENFADETYPE_Curve", EditConditionHides))
	TObjectPtr<UCurveFloat> SizeFadeCurve = nullptr;

	bool HasNoSize() const { return Brush != nullptr ? BrushSize.GetMin() <= 0 : LineSize <= 0.f; }
};

USTRUCT(BlueprintType)
struct FCanvasLineData {
	GENERATED_BODY()

	FCanvasLineData() {}
	FCanvasLineData(const FCanvasLineData& line, float newTime) :
	DrawingPlayer(line.DrawingPlayer), BoardID(line.BoardID), DrawnTime(newTime), StartingPoint(line.StartingPoint), StoppingPoint(line.StoppingPoint), PenData(line.PenData)
	{};
	FCanvasLineData(FName drawerName, int boardID, float time, FVector2D startingPoint, FVector2D stoppingPoint, FCanvasPenData penData = FCanvasPenData()) :
	DrawingPlayer(drawerName), BoardID(boardID), DrawnTime(time), StartingPoint(startingPoint), StoppingPoint(stoppingPoint), PenData(penData)
	{};

	/* Player that drew the line. */
	UPROPERTY(BlueprintReadOnly, Category = "Line")
	FName DrawingPlayer = FName();
	/* The unique id used to identify boards of the same player. */
	UPROPERTY(BlueprintReadOnly, Category = "Line")
	int BoardID = 0;
	/* The game time at which the line was drawn. */
	UPROPERTY(BlueprintReadOnly, Category = "Line")
	float DrawnTime = 0.f;
	/* The starting point of the line. */
	UPROPERTY(BlueprintReadOnly, Category = "Line")
	FVector2D StartingPoint = FVector2D::ZeroVector;
	/* The stopping point of the line. */
	UPROPERTY(BlueprintReadOnly, Category = "Line")
	FVector2D StoppingPoint = FVector2D::ZeroVector;
	/* Pen data that is used for drawing the line.  */
	UPROPERTY(BlueprintReadOnly, Category = "Line")
	FCanvasPenData PenData = FCanvasPenData();
	
	/* Returns how long since the line has been drawn. */
	float GetLineLifetime(UObject* worldContext) const;
	
};
