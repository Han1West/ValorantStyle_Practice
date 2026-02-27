// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SettingMenuWidget.h"
#include "Player/ValorantPlayer.h"
#include "Player/ValorantPlayerController.h"

#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/SpinBox.h"
#include "Kismet/GameplayStatics.h"

void USettingMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (VolumeSlider)
	{
		VolumeSlider->OnValueChanged.RemoveDynamic(this, &USettingMenuWidget::OnVolumeSliderChanged);
		VolumeSlider->OnValueChanged.AddDynamic(this, &USettingMenuWidget::OnVolumeSliderChanged);
	}
	if (SensitivitySlider)
	{
		SensitivitySlider->OnValueChanged.RemoveDynamic(this, &USettingMenuWidget::OnSensitivitySliderChanged);
		SensitivitySlider->OnValueChanged.AddDynamic(this, &USettingMenuWidget::OnSensitivitySliderChanged);
	}
	if (SensitivitySpinBox)
	{
		SensitivitySpinBox->OnValueChanged.RemoveDynamic(this, &USettingMenuWidget::OnSensitivitySpinBoxChanged);
		SensitivitySpinBox->OnValueChanged.AddDynamic(this, &USettingMenuWidget::OnSensitivitySpinBoxChanged);
	}
	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.RemoveDynamic(this, &USettingMenuWidget::OnConfirmClicked);
		ConfirmButton->OnClicked.AddDynamic(this, &USettingMenuWidget::OnConfirmClicked);
	}

	if (AValorantPlayerController* PC = GetOwningPlayer<AValorantPlayerController>())
	{
		UpdateSensitivity(PC->MouseSensitivity);
		VolumeSlider->SetValue(PC->MasterVolume);
	}
}

void USettingMenuWidget::OnVolumeSliderChanged(float Value)
{
	if (AValorantPlayerController* PC = GetOwningPlayer<AValorantPlayerController>())
	{
		PC->MasterVolume = Value;

		PC->ApplyMasterVolume(); 
		PC->SaveSettings();       
	}
}

void USettingMenuWidget::OnSensitivitySliderChanged(float Value)
{
	UpdateSensitivity(Value);
}

void USettingMenuWidget::OnSensitivitySpinBoxChanged(float Value)
{
	UpdateSensitivity(Value);
}

void USettingMenuWidget::OnConfirmClicked()
{
	// UI 종료 및 PauseMenu UI 활성화
	AValorantPlayerController* PC = GetOwningPlayer<AValorantPlayerController>();
	if (!PC)
	{
		return;
	}
	PC->ToggleSettingMenu();
	PC->TogglePauseMenu();	
}

void USettingMenuWidget::UpdateSensitivity(float Value)
{
	if (bUpdatingSensitivity) return;

	bUpdatingSensitivity = true;

	if (SensitivitySlider)
		SensitivitySlider->SetValue(Value);

	if (SensitivitySpinBox)
		SensitivitySpinBox->SetValue(Value);

	if (AValorantPlayerController* PC = GetOwningPlayer<AValorantPlayerController>())
	{
		PC->MouseSensitivity = Value;
		PC->SaveSettings();
	}

	bUpdatingSensitivity = false;
}
