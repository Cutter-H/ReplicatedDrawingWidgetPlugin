// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplicatedCanvasWidgetBase.h"

#include "EngineUtils.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "CanvasHelpers/CanvasPlayerStateHelper.h"
#include "CanvasHelpers/ReplicatedCanvasManager.h"
#include "CanvasHelpers/WorldCanvasSubsystem.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"


void UReplicatedCanvasWidgetBase::DrawLines(FPaintContext& context) const{
	 for (const FCanvasLineData& line : LineData) {
		 if(ShouldDrawLine(line)) {
		 	FLinearColor color(line.Tint);
		 	color *= Tint;
		 	const float lineLife = line.GetLineLife(GetWorld());
		 	if (FadeDuration > 0 && EraseTime > 0 && lineLife > (EraseTime - FadeDuration)) {
		 		const float fadeOpacity =  (EraseTime - lineLife) / FadeDuration;
		 		color.A *= FMath::Clamp(fadeOpacity, 0.f, 1.f);
		 	}
		 	UWidgetBlueprintLibrary::DrawSpline(context, line.StartingPoint, line.StartingDirection, line.StoppingPoint, line.StoppingDirection, color, line.Size);
		 }
	 }
 }

void UReplicatedCanvasWidgetBase::CleanupLines(){
	if (EraseTime <= 0) {
		return;
	}
	 while (LineData.Num() > 0 && LineData[0].GetLineLife(GetWorld()) > EraseTime) {
		 LineData.RemoveAt(0);
	 }
 }

// UReplicatedCanvasWidgetBase::UReplicatedCanvasWidgetBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
// 	
// }

void UReplicatedCanvasWidgetBase::TrySetupReplicator() {
	if (Replicator) { return; }
	
	if (const auto subsys = GetWorld()->GetSubsystem<UWorldCanvasSubsystem>()) {
		
		Replicator = subsys->Replicator;
		if (Replicator) {
			Replicator->OnLineAdded.AddDynamic(this, &UReplicatedCanvasWidgetBase::OnLineAddedToBoard);
		}
	}
}

void UReplicatedCanvasWidgetBase::NativeConstruct() {
	Super::NativeConstruct();

	if (!WidgetTree) { return; }

	UOverlay* root = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("OverlayRoot"));
	WidgetTree->RootWidget = root;

	DrawingPad = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("DrawingPad"));
	DrawingPad->SetVisibility(ESlateVisibility::Visible);
	DrawingPad->SetBrushColor(FLinearColor(0, 0, 0, 0));

	if (UOverlaySlot* slot = root->AddChildToOverlay(DrawingPad))	{
		slot->SetHorizontalAlignment(HAlign_Fill);
		slot->SetHorizontalAlignment(HAlign_Fill);
	}

	SetVisibility(ESlateVisibility::Visible);
	SetIsEnabled(true);
	SetIsFocusable(true);
	
}

void UReplicatedCanvasWidgetBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);
	TrySetupReplicator();
	CleanupLines();
}

int32 UReplicatedCanvasWidgetBase::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
                                               const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const {
	
	FPaintContext context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	DrawLines(context);
	if (ShouldDraw()) {
		
	}
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
			const FCanvasLineData line(GetDrawerName(), UGameplayStatics::GetTimeSeconds(GetWorld()), LastPosition, LastDirection, currentPosition, currentDirection, PenData.Size, PenData.Tint);
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
	UKismetSystemLibrary::PrintString(this, "Func Hit");

	if (board != BoardName) {
		return;
	}
	 if (true){//line.DrawingPlayer != GetDrawerName()) {
	 	LineData.Add(line);
	 	UKismetSystemLibrary::PrintString(this, "Line Added");
	 }
 }

void UReplicatedCanvasWidgetBase::RefreshFromReplicator() {
	LineData.Empty();
	if (Replicator) {
		LineData = Replicator->GetCanvasLines(BoardName);
	}
}

void UReplicatedCanvasWidgetBase::CleanBoard() {
	LineData.Empty();
	if (Replicator) {
		Replicator->CleanBoard(BoardName);
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
		DrawerName = FName(GetOwningPlayerState(true)->GetPlayerName());
	}
	return DrawerName;
}

bool UReplicatedCanvasWidgetBase::ShouldDraw_Implementation() const {
	
	if (!bWantsToDraw || !(PenData.Size > 0) || !(PenData.Tint.A > 0)) {
		return false;
	}
	
	return true;
}