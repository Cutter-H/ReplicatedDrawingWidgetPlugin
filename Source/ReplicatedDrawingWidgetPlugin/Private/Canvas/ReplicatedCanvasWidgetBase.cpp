// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplicatedCanvasWidgetBase.h"
#include "CanvasHelpers/CanvasPlayerStateHelper.h"
#include "CanvasHelpers/ReplicatedCanvasManager.h"
#include "CanvasHelpers/WorldCanvasSubsystem.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"

#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

#include "Curves/CurveFloat.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Slate/SlateBrushAsset.h"


void UReplicatedCanvasWidgetBase::DrawLines(FPaintContext& context) const{
	FVector2D currentPos = FVector2D::ZeroVector;
	FVector2D lineDirection = FVector2D::ZeroVector;
	for (const FCanvasLineData& line : LineData) {
		 if(ShouldDrawLine(line)) {
		 	FLinearColor color(line.PenData.Tint);
		 	const float lineLife = line.GetLineLifetime(GetWorld());
		 	float sizeScalar = 1.f;
		 	if (FadeDuration > 0 && EraseTime > 0 && lineLife > (EraseTime - FadeDuration)) {
		 		const float fadeAmount =  ( (EraseTime - lineLife) / FadeDuration);
		 		
		 		/* Opacity fade */ {
		 			const auto opacityFadeType = line.PenData.OpacityFadeType;
		 			if (opacityFadeType == EPenDataFadeType::PENFADETYPE_Linear) {
		 				color.A *= fadeAmount;
		 			}
		 			if (opacityFadeType == EPenDataFadeType::PENFADETYPE_Curve) {
		 				if (line.PenData.OpacityFadeCurve) {
		 					color.A *= FMath::Abs(line.PenData.OpacityFadeCurve->GetFloatValue(1-fadeAmount));
		 				}
		 			}
		 		
		 		}
		 		
		 		/* Size Fade */ {
		 			const auto sizeFadeType = line.PenData.SizeFadeType;
		 			if (sizeFadeType == EPenDataFadeType::PENFADETYPE_Linear) {
		 				sizeScalar *= fadeAmount;
		 			}
		 			if (sizeFadeType == EPenDataFadeType::PENFADETYPE_Curve) {
		 				if (line.PenData.SizeFadeCurve) {
		 					sizeScalar *= FMath::Abs(line.PenData.SizeFadeCurve->GetFloatValue(1-fadeAmount));
		 				}
		 			}
		 		
		 		}
		 	}
		 	if (line.PenData.Brush) {
		 		const FVector2D brushSize = line.PenData.BrushSize * sizeScalar;
		 		currentPos = line.StartingPoint;
		 		lineDirection = (line.StoppingPoint - currentPos).GetSafeNormal();
		 		float rotationAngle = 0.f;
		 		if (line.PenData.bBrushFollowsLine) {
		 			rotationAngle = FMath::RadiansToDegrees(FMath::Atan2(lineDirection.X, -lineDirection.Y));
		 			while (rotationAngle < 0.f) { rotationAngle += 360.f; }
		 			while (rotationAngle > 360.f) { rotationAngle -= 360.f; }
		 		}
		 		while (currentPos != line.StoppingPoint) {
		 			MakeRotatedBrush(context, line.PenData, brushSize, currentPos, rotationAngle);
		 			if ((currentPos - line.StoppingPoint).Size() <= line.PenData.StepSize) {
		 				currentPos = line.StoppingPoint;
		 			}
		 			else {
		 				currentPos += lineDirection * line.PenData.StepSize;
		 			}
		 		}
		 		MakeRotatedBrush(context, line.PenData, brushSize, currentPos, rotationAngle);
		 	}
		 	else {
		 		UWidgetBlueprintLibrary::DrawLine(context, line.StartingPoint, line.StoppingPoint, color, false, sizeScalar * line.PenData.LineSize);
		 	}
		 }
	 }
 }

void UReplicatedCanvasWidgetBase::CleanupLines(){
	if (EraseTime <= 0) {
		return;
	}
	 while (LineData.Num() > 0 && LineData[0].GetLineLifetime(GetWorld()) > EraseTime) {
		 LineData.RemoveAt(0);
	 }
 }

void UReplicatedCanvasWidgetBase::TrySetupCanvasManager() {
	if (CanvasManager) { return; }
	
	if (const auto subsys = GetWorld()->GetSubsystem<UWorldCanvasSubsystem>()) {
		
		CanvasManager = subsys->CanvasManager;
		if (CanvasManager) {
			CanvasManager->OnLineAdded.AddDynamic(this, &UReplicatedCanvasWidgetBase::OnLineAddedToBoard);
			RefreshFromManager();
		}
	}
}

void UReplicatedCanvasWidgetBase::MakeRotatedBrush(FPaintContext& context, const FCanvasPenData& penData, FVector2D brushSize, FVector2D pos, float rotation) const {
	context.MaxLayer++;
	FSlateDrawElement::MakeRotatedBox(
		context.OutDrawElements,
	   context.MaxLayer,
	   context.AllottedGeometry.ToPaintGeometry(brushSize, FSlateLayoutTransform(pos - (brushSize/2))),
	   &penData.Brush->Brush,
	   ESlateDrawEffect(penData.DrawEffect),
	   rotation,
	   TOptional<FVector2f>(),
	   FSlateDrawElement::RelativeToElement,
	   penData.Tint
   );
}

void UReplicatedCanvasWidgetBase::NativeConstruct() {
	Super::NativeConstruct();

	if (!WidgetTree) { return; }

	UOverlay* root = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("OverlayRoot"));
	WidgetTree->RootWidget = root;

	UBorder* DrawingPad = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("DrawingPad"));
	DrawingPad->SetVisibility(ESlateVisibility::Visible);
	DrawingPad->SetBrushColor(FLinearColor(0, 0, 0, 0));

	if (UOverlaySlot* slot = root->AddChildToOverlay(DrawingPad))	{
		slot->SetHorizontalAlignment(HAlign_Fill);
		slot->SetHorizontalAlignment(HAlign_Fill);
	}

	SetVisibility(ESlateVisibility::Visible);
	SetIsEnabled(true);
	SetIsFocusable(true);

	if (!IsDesignTime() && BoardID == 0) {
		BoardID = GetUniqueID();
	}
}

void UReplicatedCanvasWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);
	TrySetupCanvasManager();
	CleanupLines();
}

int32 UReplicatedCanvasWidgetBase::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
                                               const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const {
	
	FPaintContext context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	DrawLines(context);
	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}

#pragma region Mouse Overrides
FReply UReplicatedCanvasWidgetBase::NativeOnMouseButtonDown(const FGeometry& InGeometry,	const FPointerEvent& InMouseEvent) {
	if (InMouseEvent.GetEffectingButton() == FKey("LeftMouseButton")) {
		bWantsToDraw = true;
		
	}
	
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UReplicatedCanvasWidgetBase::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
	UKismetSystemLibrary::PrintString(this, "We hit");
	if (ShouldDraw()) {
		
		FVector2D currentPosition = InGeometry.AbsoluteToLocal( InMouseEvent.GetScreenSpacePosition());
		FVector2D currentDirection = FVector2D::ZeroVector;
		if (InMouseEvent.IsPointerEvent()) {
			currentDirection = InMouseEvent.GetCursorDelta();
		}
		if (InMouseEvent.IsTouchEvent()) {
			currentDirection = InMouseEvent.GetGestureDelta();
		}
		if (bStartedDrawing) {
			const FCanvasLineData line(GetDrawerName(), BoardID, UGameplayStatics::GetTimeSeconds(GetWorld()), LastPosition, currentPosition, PenData);
			LineData.Add(line);
			AddLineToState(line);
		}
		else {
			bStartedDrawing = true;
		}
		LastPosition = currentPosition;
		LastDirection = currentDirection;	
	}	
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

FReply UReplicatedCanvasWidgetBase::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) {
	bWantsToDraw = false;
	bStartedDrawing = false;
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UReplicatedCanvasWidgetBase::NativeOnMouseLeave(const FPointerEvent& InMouseEvent) {
	Super::NativeOnMouseLeave(InMouseEvent);
	bWantsToDraw = false;
	bStartedDrawing = false;
}
#pragma endregion Mouse Overrides

void UReplicatedCanvasWidgetBase::OnLineAddedToBoard(FName board, const FCanvasLineData& line) {
	if (board != BoardName) {
		return;
	}
	 if (line.DrawingPlayer != GetDrawerName() && BoardID != line.BoardID) {
	 	LineData.Add(FCanvasLineData(line, UGameplayStatics::GetTimeSeconds(GetWorld())));
	 }
 }

void UReplicatedCanvasWidgetBase::RefreshFromManager() {
	LineData.Empty();
	if (CanvasManager) {
		LineData = CanvasManager->GetCanvasLines(BoardName);
	}
}

void UReplicatedCanvasWidgetBase::CleanBoard() {
	LineData.Empty();
	if (GetPlayerStateHelper()) {
		GetPlayerStateHelper()->CleanBoard(BoardName);
	}
}

void UReplicatedCanvasWidgetBase::AddLineToState(const FCanvasLineData& line) {
	if (!StateHelper) {
		if (APlayerState* state = GetOwningPlayerState()) {
			StateHelper = Cast<UCanvasPlayerStateHelper>(state->GetComponentByClass(UCanvasPlayerStateHelper::StaticClass()));
		}
	}
	
	if (StateHelper) {
		
		StateHelper->AddLine(BoardName, line);
	}
}

FName UReplicatedCanvasWidgetBase::GetDrawerName_Implementation() {
	if (DrawerName == FName()) {
		if (GetOwningPlayerState()) {
			DrawerName = FName(GetOwningPlayerState()->GetPlayerName());
		}
	}
	return DrawerName;
}

bool UReplicatedCanvasWidgetBase::ShouldDraw_Implementation() const {
	
	if (!bWantsToDraw || PenData.HasNoSize() || !(PenData.Tint.A > 0)) {
		return false;
	}
	
	return true;
}