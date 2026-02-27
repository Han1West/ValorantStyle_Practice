// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PauseMenuWidget.h"
#include "Player/ValorantPlayer.h"
#include "Player/ValorantPlayerController.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ContinueButton)
	{
		ContinueButton->OnClicked.RemoveDynamic(this, &UPauseMenuWidget::OnContinueGameClicked);
		ContinueButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnContinueGameClicked);
	}
	if (SettingButton)
	{
		SettingButton->OnClicked.RemoveDynamic(this, &UPauseMenuWidget::OnSettingClicked);
		SettingButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnSettingClicked);
	}
	if (LobbyButton)
	{
		LobbyButton->OnClicked.RemoveDynamic(this, &UPauseMenuWidget::OnLobbyClicked);
		LobbyButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnLobbyClicked);
	}
	if (ExitButton)
	{
		ExitButton->OnClicked.RemoveDynamic(this, &UPauseMenuWidget::OnExitClicked);
		ExitButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnExitClicked);
	}

}

void UPauseMenuWidget::OnContinueGameClicked()
{
	// UI 종료
	AValorantPlayerController* PC = GetOwningPlayer<AValorantPlayerController>();
	if (!PC)
	{
		return;
	}
	PC->TogglePauseMenu();
}

void UPauseMenuWidget::OnSettingClicked()
{
	// UI 종료 및 Setting UI 활성화
	AValorantPlayerController* PC = GetOwningPlayer<AValorantPlayerController>();
	if (!PC)
	{
		return;
	}
	PC->TogglePauseMenu();
	PC->ToggleSettingMenu();
}

void UPauseMenuWidget::OnLobbyClicked()
{
	UGameplayStatics::OpenLevel(this, FName("Lobby"));
}

void UPauseMenuWidget::OnExitClicked()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
	}
}
