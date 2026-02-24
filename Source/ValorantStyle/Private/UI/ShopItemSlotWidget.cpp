// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopItemSlotWidget.h"

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

void UShopItemSlotWidget::SetItemData(UTexture2D* Icon, const FString& Name, int32 Price, EShopItemType Type)
{
	if (ItemIcon)
	{
		ItemIcon->SetBrushFromTexture(Icon);
	}
	if (ItemName)
	{
		ItemName->SetText(FText::FromString(Name));
	}
	if (ItemPriceText)
	{
		ItemPriceText->SetText(FText::Format(FText::FromString(TEXT("$ {0}")), Price));
	}

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
	if (ItemName)
	{
		ItemName->SetText(FText::FromString(Name));
	}
	if (ItemPriceText)
	{
		ItemPriceText->SetText(FText::Format(FText::FromString(TEXT("$ {0}")), Price));		
	}

	ItemType = Type;
	ItemPrice = Price;
}

void UShopItemSlotWidget::SetSlotEnabled(bool bEnabled)
{
	if (ItemButton)
	{
		ItemButton->SetIsEnabled(bEnabled);		
	}
}


void UShopItemSlotWidget::OnItemClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Item Clicekd"));
	UE_LOG(LogTemp, Display, TEXT("Price : %d"), ItemPrice);

	switch (ItemType)
	{
	case EShopItemType::Weapon:

		break;
	case EShopItemType::Shield:

		break;
	case EShopItemType::Skill:

		break;
	case EShopItemType::NONE:

		break;
	default:
		break;
	}
}
