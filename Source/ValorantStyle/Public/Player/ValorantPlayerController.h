// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ValorantPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class VALORANTSTYLE_API AValorantPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;
	
public:
	virtual void GameHasEnded(class AActor* EndGameFocus = nullptr, bool bIsWinner = false) override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> HUDClass;

	UPROPERTY(VisibleAnywhere)
	UUserWidget* HUD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UShopRootWidget> ShopWidgetClass;

	UPROPERTY(VisibleAnywhere)
	UShopRootWidget* ShopWidgetInstance;

	bool bShopOpen = false;
private:
	void ToggleShop();
};
