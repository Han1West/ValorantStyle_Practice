// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ValorantPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UI/ShopRootWidget.h"
#include "UI/CharacterSelectRootWidget.h"
#include "Player/ValorantPlayer.h"

void AValorantPlayerController::BeginPlay()
{
	Super::BeginPlay();

	HUD = CreateWidget(this, HUDClass);
	if (HUD != nullptr)
	{
		HUD->AddToViewport();
	}
}

void AValorantPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindKey(EKeys::B, IE_Pressed, this, &AValorantPlayerController::ToggleShop);
	InputComponent->BindKey(EKeys::F, IE_Pressed, this, &AValorantPlayerController::ToggleCharacterSelect);
}

void AValorantPlayerController::GameHasEnded(AActor* EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);

	HUD->RemoveFromViewport();
}

void AValorantPlayerController::ToggleShop()
{
	if (!ShopWidgetInstance)
	{
		ShopWidgetInstance = CreateWidget<UShopRootWidget>(this, ShopWidgetClass);
	}

	bShopOpen = !bShopOpen;

	if (bShopOpen)
	{
		ShopWidgetInstance->AddToViewport();		
		ShopWidgetInstance->InitializeShop();

		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(ShopWidgetInstance->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		SetInputMode(InputMode);
		SetShowMouseCursor(true);

		SetIgnoreMoveInput(true);
		SetIgnoreLookInput(true);
		
		if (AValorantPlayer* MyPlayer = Cast<AValorantPlayer>(GetPawn()))
		{
			MyPlayer->DisableInput(this);
		}
	}
	else
	{
		ShopWidgetInstance->RemoveFromParent();

		SetInputMode(FInputModeGameOnly());
		SetShowMouseCursor(false);

		SetIgnoreMoveInput(false);
		SetIgnoreLookInput(false);

		if (AValorantPlayer* MyPlayer = Cast<AValorantPlayer>(GetPawn()))
		{
			MyPlayer->EnableInput(this);
		}
	}
}

void AValorantPlayerController::ToggleCharacterSelect()
{
	if (!CharacterSelectWidgetInstance)
	{
		CharacterSelectWidgetInstance = CreateWidget<UCharacterSelectRootWidget>(this, CharacterSelectClass);
	}

	bCharacterSelectOpen = !bCharacterSelectOpen;

	if (bCharacterSelectOpen)
	{
		CharacterSelectWidgetInstance->AddToViewport();
		CharacterSelectWidgetInstance->InitializeCharacterSelect();

		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(CharacterSelectWidgetInstance->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		SetInputMode(InputMode);
		SetShowMouseCursor(true);

		SetIgnoreMoveInput(true);
		SetIgnoreLookInput(true);

		if (AValorantPlayer* MyPlayer = Cast<AValorantPlayer>(GetPawn()))
		{
			MyPlayer->DisableInput(this);
		}
	}
	else
	{
		CharacterSelectWidgetInstance->RemoveFromParent();

		SetInputMode(FInputModeGameOnly());
		SetShowMouseCursor(false);

		SetIgnoreMoveInput(false);
		SetIgnoreLookInput(false);

		if (AValorantPlayer* MyPlayer = Cast<AValorantPlayer>(GetPawn()))
		{
			MyPlayer->EnableInput(this);
		}
	}
}
