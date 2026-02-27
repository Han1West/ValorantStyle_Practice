// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingMenuWidget.generated.h"

class UButton;
class USlider;
class USpinBox;

/**
 * 
 */
UCLASS()
class VALORANTSTYLE_API USettingMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnVolumeSliderChanged(float Value);
	UFUNCTION()
	void OnSensitivitySliderChanged(float Value);
	UFUNCTION()
	void OnSensitivitySpinBoxChanged(float Value);
	UFUNCTION()
	void OnConfirmClicked();

	void UpdateSensitivity(float Value);

private:	
	UPROPERTY(meta = (BindWidget))
	USlider* VolumeSlider;
	UPROPERTY(meta = (BindWidget))
	USlider* SensitivitySlider;
	UPROPERTY(meta = (BindWidget))
	USpinBox* SensitivitySpinBox;


	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmButton;

	bool bUpdatingSensitivity = false;
};
