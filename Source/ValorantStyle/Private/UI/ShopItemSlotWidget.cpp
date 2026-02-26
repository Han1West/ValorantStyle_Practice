// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItemSlotWidget.h"
#include "Player/ValorantPlayer.h"
#include "Skill/SkillComponent.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"



void UShopItemSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ItemButton)
	{
		ItemButton->OnClicked.RemoveDynamic(this, &UShopItemSlotWidget::OnItemClicked);
		ItemButton->OnClicked.AddDynamic(this, &UShopItemSlotWidget::OnItemClicked);
	}
}

FReply UShopItemSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{		
		OnItemRightClicked();

		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UShopItemSlotWidget::SetItemData(UTexture2D* Icon, const FString& Name, int32 Price, EShopItemType Type)
{
	if (ItemIcon)
	{
		ItemIcon->SetBrushFromTexture(Icon);
	}
	if (ItemNameText)
	{
		ItemNameText->SetText(FText::FromString(Name));
	}
	if (ItemPriceText)
	{
		ItemPriceText->SetText(FText::Format(FText::FromString(TEXT("$ {0}")), Price));
	}

	ItemName = Name;
	ItemType = Type;
	ItemPrice = Price;
}

void UShopItemSlotWidget::SetSkillItemData(UTexture2D* Icon, const FString& Name, int32 Price, EShopItemType Type, const FString& BaseSkillName)
{
	if (ItemIcon)
	{
		ItemIcon->SetBrushFromTexture(Icon);
	}
	if (ItemNameText)
	{
		ItemNameText->SetText(FText::FromString(Name));
	}
	if (ItemPriceText)
	{
		ItemPriceText->SetText(FText::Format(FText::FromString(TEXT("$ {0}")), Price));
	}

	ItemName = BaseSkillName;
	ItemType = Type;
	ItemPrice = Price;
}

void UShopItemSlotWidget::SetWeaponItemData(UTexture2D* Icon, const FString& Name, int32 Price, EShopItemType Type)
{
	if (ItemIcon)
	{
		UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(IconMaterial, this);		
		MID->SetTextureParameterValue(TEXT("IconTexture"), Icon);
		ItemIcon->SetBrushFromMaterial(MID);
	}
	if (ItemNameText)
	{
		ItemNameText->SetText(FText::FromString(Name));
	}
	if (ItemPriceText)
	{
		ItemPriceText->SetText(FText::Format(FText::FromString(TEXT("$ {0}")), Price));		
	}

	ItemName = Name;
	ItemType = Type;
	ItemPrice = Price;
}


void UShopItemSlotWidget::OnItemClicked()
{
	AValorantPlayer* Player = GetOwningPlayerPawn<AValorantPlayer>();
	if (!Player)
	{
		return;
	}
	if (Player->GetCurrentBudget() < ItemPrice)
	{
		UE_LOG(LogTemp, Warning, TEXT("Need More Credit!"));
		return;
	}
	if (bEnabled)
	{
		return;
	}


	UE_LOG(LogTemp, Warning, TEXT("Item Clicekd"));
	// 스킬의 경우 최대개수가 안넘으면 구매 가능
	if (ItemType == EShopItemType::Skill)
	{
		if (SkillMaxCount <= SkillCurCount)
		{
			UE_LOG(LogTemp, Warning, TEXT("Cant buy More Skills!"));
			return;
		}
		else
		{
			Player->BuyItem(ItemName, ItemPrice);
		}
	}
	else
	{
		// 아이템이 장착 중이고 팔수 있는 상태면 구매 가능(새로운 무기로 교체)
		if (!Player->IsItemCanSelled(ItemName, ItemType))
		{
			Player->BuyItem(ItemName, ItemPrice);
		}
	}


}

void UShopItemSlotWidget::OnItemRightClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Right clicked"));

	AValorantPlayer* Player = GetOwningPlayerPawn<AValorantPlayer>();
	if (!Player)
	{
		return;
	}

	// 플레이어가 해당 아이템을 장착중이라면 판매 & 장착중이여도 사용한 아이템은 판매 X
	if (Player->IsItemCanSelled(ItemName, ItemType))
	{
		Player->ResellItem(ItemName, ItemPrice);
	}
}



void UShopItemSlotWidget::RefreshState()
{
	if (ItemType == EShopItemType::Skill)
	{
		RefreshStateSkills();
	}
	else
	{
		RefreshStateOthers();
	}
}

void UShopItemSlotWidget::RefreshStateSkills()
{

	AValorantPlayer* Player = GetOwningPlayerPawn<AValorantPlayer>();
	if (!Player)
	{
		return;
	}

	bool bEquipped = Player->IsItemEquipped(ItemName, ItemType);

	if (bEquipped)
	{
		UE_LOG(LogTemp, Display, TEXT("bEquipped : %s"), *ItemName);
		ItemButton->SetStyle(EqquippedStyle);
		bEnabled = false;
	}
	else
	{
		// 밴달 제외 모든 무기 비활성
		if (ItemType == EShopItemType::Weapon && ItemName != "VANDAL")
		{
			ItemButton->SetStyle(UnenabledStyle);
			bEnabled = true;
			return;
		}

		if (Player->GetCurrentBudget() < ItemPrice)
		{
			ItemButton->SetStyle(UnenabledStyle);
			bEnabled = true;
		}
		else
		{
			ItemButton->SetStyle(NormalStyle);
			bEnabled = false;
		}
	}

	// 스킬 개수 최신화
	if (ItemName == "SKILL Q")
	{
		SkillCurCount = Player->GetSkillComponent()->GetSkillQCount();
	}
	if (ItemName == "SKILL E")
	{
		SkillCurCount = Player->GetSkillComponent()->GetSkillECount();
	}
	if (ItemName == "SKILL C")
	{
		SkillCurCount = Player->GetSkillComponent()->GetSkillCCount();
	}
}

void UShopItemSlotWidget::RefreshStateOthers()
{

	AValorantPlayer* Player = GetOwningPlayerPawn<AValorantPlayer>();
	if (!Player)
	{
		return;
	}

	bool bEquipped = Player->IsItemEquipped(ItemName, ItemType);

	if (bEquipped)
	{
		UE_LOG(LogTemp, Display, TEXT("bEquipped : %s"), *ItemName);
		ItemButton->SetStyle(EqquippedStyle);
		bEnabled = false;
	}
	else
	{
		// 밴달 제외 모든 무기 비활성
		if (ItemType == EShopItemType::Weapon && ItemName != "VANDAL")
		{
			ItemButton->SetStyle(UnenabledStyle);
			bEnabled = true;
			return;
		}

		if (Player->GetCurrentBudget() < ItemPrice)
		{
			ItemButton->SetStyle(UnenabledStyle);
			bEnabled = true;
		}
		else
		{
			ItemButton->SetStyle(NormalStyle);
			bEnabled = false;
		}
	}
}