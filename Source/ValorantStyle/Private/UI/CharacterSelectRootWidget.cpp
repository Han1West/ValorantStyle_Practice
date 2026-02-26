// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterSelectRootWidget.h"
#include "UI/CharacterSlotWidget.h"
#include "Player/ValorantPlayer.h"
#include "Player/ValorantPlayerController.h"

#include "Components/OverlaySlot.h"
#include "Components/Overlay.h"
#include "Components/Button.h"

void UCharacterSelectRootWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ConfirmButton)
	{
		ConfirmButton->OnClicked.RemoveDynamic(this, &UCharacterSelectRootWidget::OnItemClicked);
		ConfirmButton->OnClicked.AddDynamic(this, &UCharacterSelectRootWidget::OnItemClicked);
	}

	AValorantPlayer* Player = GetOwningPlayerPawn<AValorantPlayer>();
	if (!Player) return;

	Player->OnCharacterSelectChanged.RemoveDynamic(this, &UCharacterSelectRootWidget::RefreshAllSlots);
	Player->OnCharacterSelectChanged.AddDynamic(this, &UCharacterSelectRootWidget::RefreshAllSlots);
}

void UCharacterSelectRootWidget::InitializeCharacterSelect()
{
	if (!CharacterSlotClass)
	{
		return;
	}

	JettOverlay->ClearChildren();
	PhoenixOverlay->ClearChildren();

	AddSlotToOverlay(
		JettOverlay,
		JettIcon,
		TEXT("JETT")
	);

	AddSlotToOverlay(
		PhoenixOverlay,
		PhoenixIcon,
		TEXT("PHOENIX")
	);
}

UCharacterSlotWidget* UCharacterSelectRootWidget::AddSlotToOverlay(UOverlay* TargetOverlay, UTexture2D* Icon, const FString& Name)
{
	if (!TargetOverlay)
	{
		return nullptr;
	}

	UCharacterSlotWidget* Character = CreateWidget<UCharacterSlotWidget>(GetWorld(), CharacterSlotClass);
	UOverlaySlot* NewSlot = TargetOverlay->AddChildToOverlay(Character);

	if (NewSlot)
	{
		NewSlot->SetHorizontalAlignment(HAlign_Fill);
		NewSlot->SetVerticalAlignment(VAlign_Fill);
	}

	Character->SetItemData(Icon, Name);
	Character->SetOwnerWidget(this);
	Slots.Add(Character);
	return Character;
}


void UCharacterSelectRootWidget::OnItemClicked()
{
	AValorantPlayer* Player = GetOwningPlayerPawn<AValorantPlayer>();
	if (!Player)
	{
		return;
	}
	AValorantPlayerController* PC = Cast<AValorantPlayerController>(Player->GetController());
	if (!PC)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Confirm!!"));

	// 캐릭터를 바꿔준다.
	if (SelectedCharacter == TEXT("JETT"))
	{
		Player->SetCharacterType(ECharacterType::Jett);
	}
	if (SelectedCharacter == TEXT("PHOENIX"))
	{
		Player->SetCharacterType(ECharacterType::Phoenix);
	}

	// 캐릭터 선택창을 끈다.
	PC->ToggleCharacterSelect();
}

void UCharacterSelectRootWidget::RefreshAllSlots()
{
	for (UCharacterSlotWidget* CurSlot : Slots)
	{
		CurSlot->RefreshState();
	}
}