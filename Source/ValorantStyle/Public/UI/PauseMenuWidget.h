// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class VALORANTSTYLE_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()
private:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnContinueGameClicked();
	UFUNCTION()
	void OnSettingClicked();
	UFUNCTION()
	void OnLobbyClicked();
	UFUNCTION()
	void OnExitClicked();

private:
	UPROPERTY(meta = (BindWidget))
	UButton* ContinueButton;
	UPROPERTY(meta = (BindWidget))
	UButton* SettingButton;
	UPROPERTY(meta = (BindWidget))
	UButton* LobbyButton;
	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;
};
