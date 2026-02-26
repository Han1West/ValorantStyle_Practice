// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterSlotWidget.h"
#include "UI/CharacterSelectRootWidget.h"
#include "Player/ValorantPlayer.h"

#include "Components/Image.h"
#include "Components/Button.h"

void UCharacterSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CharacterButton)
	{
		CharacterButton->OnClicked.RemoveDynamic(this, &UCharacterSlotWidget::OnItemClicked);
		CharacterButton->OnClicked.AddDynamic(this, &UCharacterSlotWidget::OnItemClicked);
	}
}

void UCharacterSlotWidget::RefreshState()
{
	// 자신이 선택되있다면
	if (OwnerRootWidget->GetSelectedCharacterName() == CharacterName)
	{
		CharacterButton->SetStyle(SelectedStyle);
	}
	else
	{
		CharacterButton->SetStyle(NormalStyle);
	}
}

void UCharacterSlotWidget::SetItemData(UTexture2D* Icon, const FString& Name)
{
	if (Icon)
	{
		CharacterIcon->SetBrushFromTexture(Icon);
	}
	
	CharacterName = Name;
}

void UCharacterSlotWidget::OnItemClicked()
{
	if (!OwnerRootWidget)
	{
		return;
	}
	AValorantPlayer* Player = GetOwningPlayerPawn<AValorantPlayer>();
	if (!Player)
	{
		return;
	}


	UE_LOG(LogTemp, Warning, TEXT("Character Cliocked!"));

	// 루트위젯에 선택된 정보를 알려줌
	OwnerRootWidget->SetSelectedCharacterName(CharacterName);
	// Broadcast용 함수 호출
	Player->SelectCharacter();
}