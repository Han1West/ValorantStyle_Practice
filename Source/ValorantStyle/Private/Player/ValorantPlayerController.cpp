// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ValorantPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "UI/ShopRootWidget.h"
#include "UI/CharacterSelectRootWidget.h"
#include "UI/PauseMenuWidget.h"
#include "UI/SettingMenuWidget.h"
#include "Gamemode/GameSettingSave.h"
#include "Player/ValorantPlayer.h"

#include "Kismet/GameplayStatics.h"


void AValorantPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeGameOnly());
	SetShowMouseCursor(false);

	LoadSettings();

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
	InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &AValorantPlayerController::OnESCPressed);
}

void AValorantPlayerController::GameHasEnded(AActor* EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);

	HUD->RemoveFromViewport();
}

void AValorantPlayerController::SaveSettings()
{
	if (!SettingSave)
	{
		return;
	}

	SettingSave->MouseSensitivity = MouseSensitivity;
	SettingSave->MasterVolume = MasterVolume;
	UE_LOG(LogTemp, Warning, TEXT("Saving Volume: %f"), MasterVolume);

	UGameplayStatics::SaveGameToSlot(SettingSave, TEXT("GameSettingSlot"), 0);
}

void AValorantPlayerController::LoadSettings()
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("GameSettingSlot"), 0))
	{
		SettingSave = Cast<UGameSettingSave>(UGameplayStatics::LoadGameFromSlot(TEXT("GameSettingSlot"), 0));
	}
	else
	{
		SettingSave = Cast<UGameSettingSave>(UGameplayStatics::CreateSaveGameObject(UGameSettingSave::StaticClass()));

		UGameplayStatics::SaveGameToSlot(SettingSave, TEXT("GameSettingSlot"), 0);
	}

	if (SettingSave)
	{
		MouseSensitivity = SettingSave->MouseSensitivity;
		MasterVolume = SettingSave->MasterVolume;
		UE_LOG(LogTemp, Warning, TEXT("Loaded Volume: %f"), MasterVolume);
		ApplyMasterVolume();
	}

}

void AValorantPlayerController::ApplyMasterVolume()
{
	UGameplayStatics::SetSoundMixClassOverride(
		GetWorld(),
		MasterSoundMix,
		MasterSoundClass,
		MasterVolume,
		1.f, 0.f, true);

	UGameplayStatics::PushSoundMixModifier(GetWorld(), MasterSoundMix);
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

void AValorantPlayerController::TogglePauseMenu()
{
	if (!PauseWidgetInstance)
	{
		PauseWidgetInstance = CreateWidget<UPauseMenuWidget>(this, PauseWidgetClass);
	}

	bPauseMenuOpen = !bPauseMenuOpen;

	if (bPauseMenuOpen)
	{
		PauseWidgetInstance->AddToViewport();

		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(PauseWidgetInstance->TakeWidget());
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
		PauseWidgetInstance->RemoveFromParent();

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

void AValorantPlayerController::ToggleSettingMenu()
{
	if (!SettingWidgetInstance)
	{
		SettingWidgetInstance = CreateWidget<USettingMenuWidget>(this, SettingWidgetClass);
	}

	bSettingMenuOpen = !bSettingMenuOpen;

	if (bSettingMenuOpen)
	{
		SettingWidgetInstance->AddToViewport();

		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(SettingWidgetInstance->TakeWidget());
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
		SettingWidgetInstance->RemoveFromParent();

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

void AValorantPlayerController::OnESCPressed()
{
	// 세팅 메뉴가 켜져있으면 끄고 return
	if (bSettingMenuOpen)
	{
		ToggleSettingMenu();
		return;
	}

	// 퍼즈 메뉴가 켜져있으면 끄고 return
	if (bPauseMenuOpen)
	{
		TogglePauseMenu();
		return;
	}

	// 아무것도 안켜져있으면 pausemenu 켜기
	TogglePauseMenu();
}
