// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterSlotWidget.generated.h"

class UImage;
class UButton;
class UCharacterSelectRootWidget;
/**
 * 
 */
UCLASS()
class VALORANTSTYLE_API UCharacterSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetItemData(UTexture2D* Icon, const FString& Name);

	void RefreshState();

	void SetOwnerWidget(UCharacterSelectRootWidget* Owner) { OwnerRootWidget = Owner; }
private:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnItemClicked();

private:
	UPROPERTY(meta = (BindWidget))
	UImage* CharacterIcon;
	UPROPERTY(meta = (BindWidget))
	UButton* CharacterButton;

	FString CharacterName;

	UPROPERTY(EditAnywhere, Category = "Style")
	FButtonStyle NormalStyle;
	UPROPERTY(EditAnywhere, Category = "Style")
	FButtonStyle SelectedStyle;

	UCharacterSelectRootWidget* OwnerRootWidget;
};
