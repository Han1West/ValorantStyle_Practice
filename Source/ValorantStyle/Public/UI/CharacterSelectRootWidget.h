// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterSelectRootWidget.generated.h"

class UOverlay;
class UButton;
class UCharacterSlotWidget;

/**
 * 
 */
UCLASS()
class VALORANTSTYLE_API UCharacterSelectRootWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeCharacterSelect();
	
	void SetSelectedCharacterName(const FString& Name) { SelectedCharacter = Name; }
	FString GetSelectedCharacterName() const { return SelectedCharacter; }

private:
	UCharacterSlotWidget* AddSlotToOverlay(UOverlay* TargetOverlay, UTexture2D* Icon, const FString& Name);

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnItemClicked();
	UFUNCTION()
	void RefreshAllSlots();

private:
	UPROPERTY(meta = (BindWidget))
	UOverlay* JettOverlay;
	UPROPERTY(meta = (BindWidget))
	UOverlay* PhoenixOverlay;

	UPROPERTY(meta = (BindWidget))
	UButton* ConfirmButton;

	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UCharacterSlotWidget> CharacterSlotClass;

	UPROPERTY(EditDefaultsOnly, Category = "Texture");
	UTexture2D* JettIcon;
	UPROPERTY(EditDefaultsOnly, Category = "Texture");
	UTexture2D* PhoenixIcon;

	FString SelectedCharacter = TEXT("NONE");
	
	UPROPERTY()
	TArray<UCharacterSlotWidget*> Slots;

};
