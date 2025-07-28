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

	// Basically a tick function that draws lines.
	void DrawLines(FPaintContext& context) const;
	// Tick function that deletes unused lines outside the EraseTime. 
	void CleanupLines();
	/* Tick function that attempts to set up maanger. If it's already set this does nothing. ONLY ONE CANVAS MANAGER SHOULD BE USED PER MAP. */
	void TrySetupCanvasManager();
	
	bool bWantsToDraw = false;
	bool bStartedDrawing = false;
	TArray<FCanvasLineData> LineData;
	FName DrawerName = FName();
	FVector2D LastPosition = FVector2D::ZeroVector;
	FVector2D LastDirection = FVector2D::ZeroVector;

	

	UPROPERTY()
	AReplicatedCanvasManager* CanvasManager;
	UPROPERTY()
	UCanvasPlayerStateHelper* StateHelper;
protected:
	/* Buncha overrides to the parent class for alotta thangs. */
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;
	virtual FReply NativeOnMouseButtonDown( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent ) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	/* Sends a line to the player state helper. */
	UFUNCTION()
	void AddLineToState(const FCanvasLineData& line);

	/* Override to change how the drawer is identified. By default, GetPlayerName from the player state is used, but this is included if other means should be used. */
	UFUNCTION(BlueprintNativeEvent)
	FName GetDrawerName();
	virtual FName GetDrawerName_Implementation();
	
	/* Override for custom logic to let the user draw. If this is false then no drawing occurs. */
	UFUNCTION(BlueprintNativeEvent)
	bool ShouldDraw() const;
	virtual bool ShouldDraw_Implementation() const;
	
	/* Override for custom logic to check if a line should be drawn. */
	UFUNCTION(BlueprintNativeEvent)
	bool ShouldDrawLine(const FCanvasLineData& line) const;
	virtual bool ShouldDrawLine_Implementation(const FCanvasLineData& line) const {return true;}

	/* Used to bind to the Canvas Manager to receive newly drawn lines from other players. */
	UFUNCTION()
	void OnLineAddedToBoard(FName board, const FCanvasLineData& line);

	/* This is the data the widget currently has of all the lines. These are ordered by time drawn. */
	UFUNCTION(BlueprintCallable, Category = "Canvas")
	TArray<FCanvasLineData> GetLineData() const { return LineData; }


	/* Blueprint Getter for the Canvas Manager. This is used to receive replicated data.
	 * 	
	 * How the Replication works.
	 *
	 *  CanvasManager   <------PlayerStateHelper  <--        Server Level
	 *    |                                          \
	 *    |                                          |
	 *    \------->  Widget-->  PlayerStateHelper---/        Client Level
	 */
	UFUNCTION(BlueprintCallable, Category = "Canvas")
	AReplicatedCanvasManager* GetManagerActor() const {return CanvasManager;}

	/* Blueprint Getter for the Player State component. This is used to send requests to the server.
	 *
	 * How the Replication works.
	 *
	 *  CanvasManager   <------PlayerStateHelper  <--        Server Level
	 *    |                                          \
	 *    |                                          |
	 *    \------->  Widget-->  PlayerStateHelper---/        Client Level
	 */
	UFUNCTION(BlueprintCallable, Category = "Canvas")
	UCanvasPlayerStateHelper* GetPlayerStateHelper() const {return StateHelper;}

	/* Rebuilds the board to match the lines found in the Canvas Manager. */
	UFUNCTION(BlueprintCallable, Category = "Canvas")
	void RefreshFromManager();

	/* Erases all lines on the board. */
	UFUNCTION(BlueprintCallable, Category = "Canvas")
	void CleanBoard();
	
	
public:
	/* Used to identify this board between players. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drawing", meta=(ExposeOnSpawn = true))
	FName BoardName;
	/* Used to identify this board between instances on the same player. If left as 0 it will be given a unique id. */
	UPROPERTY(BlueprintReadOnly, Category="Drawing", meta=(ExposeOnSpawn = true))
	int BoardID = 0;
	/* This is used as parameters to draw lines. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drawing")
	FCanvasPenData PenData;
	/* Amount of time in seconds before lines disappear. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drawing")
	float EraseTime = 0.f;
	/* Amount of time prior to being erased where the lines will fade. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drawing", meta=(EditCondition))
	float FadeDuration = 0.f;
};

