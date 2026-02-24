// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopItemSlotWidget.generated.h"

class UImage;
class UTextBlock;
class UButton;

UENUM(BlueprintType)
enum class EShopItemType : uint8
{
	Weapon,
	Shield,
	Skill,

	NONE,
};

/**
 * 
 */
UCLASS()
class VALORANTSTYLE_API UShopItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetItemData(UTexture2D* Icon, const FString& Name, int32 Price, EShopItemType Type);
	void SetWeaponItemData(UTexture2D* Icon, const FString& Name, int32 Price, EShopItemType Type);

	void SetSlotEnabled(bool bEnabled);

	void SetSkillMaxCount(int32 SkillCount) { SkillMaxCount = SkillCount; }
	void SetSkillCurCount(int32 SkillCount) { SkillCurCount = SkillCount; }

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemIcon;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemName;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemPriceText;
	UPROPERTY(meta = (BindWidget))
	UButton* ItemButton;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	class UMaterialInterface* IconMaterial;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 SkillMaxCount = 0;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 SkillCurCount = 0;

	int32 ItemPrice = 0;

	EShopItemType ItemType = EShopItemType::NONE;

protected:
	UFUNCTION()
	void OnItemClicked();

};
