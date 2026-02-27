// Fill out your copyright notice in the Description page of Project Settings.


#include "Gamemode/LobbyGameMode.h"
#include "UI/LobbyWidget.h"

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (LobbyWidgetClass)
	{
		ULobbyWidget* LobbyWidget = CreateWidget<ULobbyWidget>(GetWorld(), LobbyWidgetClass);
		if (LobbyWidget)
		{
			LobbyWidget->AddToViewport();

			APlayerController* PC = GetWorld()->GetFirstPlayerController();
			if (PC)
			{
				PC->SetShowMouseCursor(true);
				PC->SetInputMode(FInputModeUIOnly());
			}
		}
	}
}
