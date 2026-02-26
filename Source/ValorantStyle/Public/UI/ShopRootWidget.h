// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ShopRootWidget.generated.h"

class UGridPanel;
class UShopItemSlotWidget;
class UGridSlot;
class UOverlay;
enum class EShopItemType : uint8;
/**
 * 
 */
UCLASS()
class VALORANTSTYLE_API UShopRootWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeShop();

protected:
	//UGridSlot* AddCategoryHeader(const FString& Title, int32 Row, int32 col);
	
	UShopItemSlotWidget* AddSlotToOverlay(UOverlay* TargetOverlay, UTexture2D* Icon, const FString& Name, int32 Price, EShopItemType Type, bool CanBuy);
	UShopItemSlotWidget* AddShieldSlotToOverlay(UOverlay* TargetOverlay, UTexture2D* Icon, const FString& Name, int32 Price, EShopItemType Type, bool CanBuy);
	UShopItemSlotWidget* AddSkillSlotToOverlay(UOverlay* TargetOverlay, UTexture2D* Icon, const FString& Name, int32 Price, EShopItemType Type, const FString& SkilBaseName, bool CanBuy, int32 SkillMaxCount = 0, int32 SkillCurCount = 0);

private:
	void ClearAllOverlay();
	void IntializeWeaponOverlay();
	void IntializeShiledOverlay();
	void IntializeSkillOverlay();
	
	UFUNCTION()
	void RefreshAllSlots();

private:
	UPROPERTY(meta = (BindWidget))
	UOverlay* ClassicOverlay;
	UPROPERTY(meta = (BindWidget))
	UOverlay* ShortyOverlay;
	UPROPERTY(meta = (BindWidget))
	UOverlay* FrenzyOverlay;
	UPROPERTY(meta = (BindWidget))
	UOverlay* GhostOverlay;
	UPROPERTY(meta = (BindWidget))
	UOverlay* SheriffOverlay;

	UPROPERTY(meta = (BindWidget))
	UOverlay* StingerOverlay;
	UPROPERTY(meta = (BindWidget))
	UOverlay* SpectreOverlay;
	UPROPERTY(meta = (BindWidget))
	UOverlay* BuckyOverlay;
	UPROPERTY(meta = (BindWidget))
	UOverlay* JudgeOverlay;

	UPROPERTY(meta = (BindWidget))
	UOverlay* BulldogOverlay;
	UPROPERTY(meta = (BindWidget))
	UOverlay* GuardianOverlay;
	UPROPERTY(meta = (BindWidget))
	UOverlay* PhantomOverlay;
	UPROPERTY(meta = (BindWidget))
	UOverlay* VandalOverlay;

	UPROPERTY(meta = (BindWidget))
	UOverlay* MarshalOverlay;
	UPROPERTY(meta = (BindWidget))
	UOverlay* OperatorOverlay;
	UPROPERTY(meta = (BindWidget))
	UOverlay* AresOverlay;
	UPROPERTY(meta = (BindWidget))
	UOverlay* OdinOverlay;

	UPROPERTY(meta = (BindWidget))
	UOverlay* LightShieldOverlay;

	UPROPERTY(meta = (BindWidget))
	UOverlay* HeavyShieldOverlay;

	UPROPERTY(meta = (BindWidget))
	UOverlay* SkillQOverlay;

	UPROPERTY(meta = (BindWidget))
	UOverlay* SkillEOverlay;

	UPROPERTY(meta = (BindWidget))
	UOverlay* SkillCOverlay;


	UPROPERTY(EditAnywhere)
	TSubclassOf<UShopItemSlotWidget> ShopItemSlotClass;	

	UPROPERTY(EditAnywhere)
	TSubclassOf<UShopItemSlotWidget> ShopSkillItemSlotClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UShopItemSlotWidget> ShopShieldItemSlotClass;

	UPROPERTY()
	TArray<UShopItemSlotWidget*> Slots;


	UPROPERTY(EditDefaultsOnly, Category = "Texture");
	UTexture2D* ClassicIcon;
	UPROPERTY(EditDefaultsOnly, Category = "Texture");
	UTexture2D* ShortyIcon;
	UPROPERTY(EditDefaultsOnly, Category = "Texture");
	UTexture2D* FrenzyIcon;
	UPROPERTY(EditDefaultsOnly, Category = "Texture");
	UTexture2D* GhostIcon;
	UPROPERTY(EditDefaultsOnly, Category = "Texture");
	UTexture2D* SheriffIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Texture");
	UTexture2D* StingerIcon;	
	UPROPERTY(EditDefaultsOnly, Category = "Texture");
	UTexture2D* SpectreIcon;	
	UPROPERTY(EditDefaultsOnly, Category = "Texture");
	UTexture2D* BuckyIcon;
	UPROPERTY(EditDefaultsOnly, Category = "Texture");
	UTexture2D* JudgeIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Texture");
	UTexture2D* BulldogIcon;
	UPROPERTY(EditDefaultsOnly, Category = "Texture");
	UTexture2D* GuardianIcon;
	UPROPERTY(EditDefaultsOnly, Category = "Texture");
	UTexture2D* PhantomIcon;
	UPROPERTY(EditDefaultsOnly, Category = "Texture");
	UTexture2D* VandalIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Texture");
	UTexture2D* MarshalIcon;
	UPROPERTY(EditDefaultsOnly, Category = "Texture");
	UTexture2D* OperatorIcon;
	UPROPERTY(EditDefaultsOnly, Category = "Texture");
	UTexture2D* AresIcon;
	UPROPERTY(EditDefaultsOnly, Category = "Texture");
	UTexture2D* OdinIcon;
	
	UPROPERTY(EditDefaultsOnly, Category = "Texture");
	UTexture2D* LightShieldIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Texture");
	UTexture2D* HeavyShieldIcon;

	int32 PlayerBudget = 0;
};
