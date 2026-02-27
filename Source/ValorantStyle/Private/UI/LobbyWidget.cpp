// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LobbyWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnStartClicked);
	}
	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &ULobbyWidget::OnQuitClicked);
	}
}


void ULobbyWidget::OnStartClicked()
{
	UGameplayStatics::OpenLevel(this, FName("Main"));
}

void ULobbyWidget::OnQuitClicked()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
	}
}
