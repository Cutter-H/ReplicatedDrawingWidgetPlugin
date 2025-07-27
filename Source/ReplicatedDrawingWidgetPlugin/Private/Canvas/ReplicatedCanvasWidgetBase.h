// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CanvasHelpers/ReplicatedCanvasData.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "ReplicatedCanvasWidgetBase.generated.h"

class UCanvasPlayerStateHelper;
class AReplicatedCanvasManager;
class USizeBox;
class UBorder;
/**
 * 
 */
UCLASS()
class REPLICATEDDRAWINGWIDGETPLUGIN_API UReplicatedCanvasWidgetBase : public UUserWidget {
	GENERATED_BODY()

	void DrawLines(FPaintContext& context) const;
	void CleanupLines();

	bool bWantsToDraw = false;
	bool bStartedDrawing = false;
	TArray<FCanvasLineData> LineData;
	FName DrawerName = FName();
	FVector2D LastPosition = FVector2D::ZeroVector;
	FVector2D LastDirection = FVector2D::ZeroVector;

	bool LastPointValid() const { return (LastPosition + LastDirection).Length() > 0; }

	void TrySetupReplicator();
	
	AReplicatedCanvasManager* Replicator;
	UCanvasPlayerStateHelper* StateHelper;
protected:
	UPROPERTY()
	USizeBox* RootBox;
	UPROPERTY()
	UBorder* DrawingPad;
	//UReplicatedCanvasWidgetBase(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;
	
	virtual FReply NativeOnMouseButtonDown( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent ) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	UFUNCTION()
	void AddLineToState(const FCanvasLineData& line);
	
	UFUNCTION(BlueprintNativeEvent)
	FName GetDrawerName();
	UFUNCTION(BlueprintNativeEvent)
	bool ShouldDraw() const;

	UFUNCTION(BlueprintNativeEvent)
	bool ShouldDrawLine(const FCanvasLineData& line) const;
	bool ShouldDrawLine_Implementation(const FCanvasLineData& line) const {return true;}

	UFUNCTION()
	void OnLineAddedToBoard(FName board, const FCanvasLineData& line);

	UFUNCTION(BlueprintCallable)
	TArray<FCanvasLineData> GetLineData() const { return LineData; }

	UFUNCTION(BlueprintCallable)
	AReplicatedCanvasManager* GetReplicatorActor() const {return Replicator;}

	UFUNCTION(BlueprintCallable)
	void RefreshFromReplicator();

	UFUNCTION(BlueprintCallable)
	void CleanBoard();
	
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drawing", meta=(ExposeOnSpawn = true))
	FName BoardName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drawing", meta=(ExposeOnSpawn = true))
	FLinearColor Tint = FLinearColor::White;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drawing")
	FCanvasPenData PenData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drawing")
	float EraseTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drawing")
	float FadeDuration = 0.f;
};

