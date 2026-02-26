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
	void SetSkillItemData(UTexture2D* Icon, const FString& Name, int32 Price, EShopItemType Type, const FString& BaseSkillName);
	void SetWeaponItemData(UTexture2D* Icon, const FString& Name, int32 Price, EShopItemType Type);

	void SetSkillMaxCount(int32 SkillCount) { SkillMaxCount = SkillCount; }
	void SetSkillCurCount(int32 SkillCount) { SkillCurCount = SkillCount; }

	void RefreshState();
protected:
	UPROPERTY(meta = (BindWidget))
	UImage* ItemIcon;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemNameText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemPriceText;
	UPROPERTY(meta = (BindWidget))
	UButton* ItemButton;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	class UMaterialInterface* IconMaterial;

	UPROPERTY(EditAnywhere, Category ="Style")
	FButtonStyle NormalStyle;
	UPROPERTY(EditAnywhere, Category = "Style")
	FButtonStyle EqquippedStyle;
	UPROPERTY(EditAnywhere, Category = "Style")
	FButtonStyle UnenabledStyle;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 SkillMaxCount = 0;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 SkillCurCount = 0;

	FString ItemName;
	int32 ItemPrice = 0;

	EShopItemType ItemType = EShopItemType::NONE;
	bool bEnabled = false;

protected:
	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFUNCTION()
	void OnItemClicked();
	
	UFUNCTION()
	void OnItemRightClicked();	

	void RefreshStateSkills();;
	void RefreshStateOthers();
};
