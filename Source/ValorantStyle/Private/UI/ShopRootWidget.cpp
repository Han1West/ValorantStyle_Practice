// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopRootWidget.h"
#include "UI/ShopItemSlotWidget.h"
#include "UI/CategoryHeaderWidget.h"
#include "Player/ValorantPlayer.h"
#include "Skill/SkillComponent.h"

#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/OverlaySlot.h"
#include "Components/Overlay.h"

void UShopRootWidget::InitializeShop()
{
	if (!ShopItemSlotClass)
	{
		return;
	}



	ClearAllOverlay();

	IntializeSkillOverlay();
	IntializeWeaponOverlay();
	IntializeShiledOverlay();	
}


UShopItemSlotWidget* UShopRootWidget::AddSlotToOverlay(UOverlay* TargetOverlay, UTexture2D* Icon, const FString& Name, int32 Price, EShopItemType Type)
{
	if (!TargetOverlay)
	{
		return nullptr;
	}

	UShopItemSlotWidget* Item = CreateWidget<UShopItemSlotWidget>(GetWorld(), ShopItemSlotClass);
	UOverlaySlot* NewSlot = TargetOverlay->AddChildToOverlay(Item);

	if (NewSlot)
	{
		NewSlot->SetHorizontalAlignment(HAlign_Fill);
		NewSlot->SetVerticalAlignment(VAlign_Fill);
	}

	Item->SetWeaponItemData(Icon, Name, Price, Type);

	if (PlayerBudget < Price)
	{
		Item->SetSlotEnabled(false);
	}
	else
	{
		Item->SetSlotEnabled(true);
	}
	
	return Item;
}

UShopItemSlotWidget* UShopRootWidget::AddShieldSlotToOverlay(UOverlay* TargetOverlay, UTexture2D* Icon, const FString& Name, int32 Price, EShopItemType Type)
{
	if (!TargetOverlay)
	{
		return nullptr;
	}

	UShopItemSlotWidget* Item = CreateWidget<UShopItemSlotWidget>(GetWorld(), ShopShieldItemSlotClass);
	UOverlaySlot* NewSlot = TargetOverlay->AddChildToOverlay(Item);

	if (NewSlot)
	{
		NewSlot->SetHorizontalAlignment(HAlign_Fill);
		NewSlot->SetVerticalAlignment(VAlign_Fill);
	}

	Item->SetItemData(Icon, Name, Price, Type);

	if (PlayerBudget < Price)
	{
		Item->SetSlotEnabled(false);
	}
	else
	{
		Item->SetSlotEnabled(true);
	}

	return Item;
}


UShopItemSlotWidget* UShopRootWidget::AddSkillSlotToOverlay(UOverlay* TargetOverlay, UTexture2D* Icon, const FString& Name, int32 Price, EShopItemType Type, int32 SkillMaxCount, int32 SkillCurCount)
{
	if (!TargetOverlay)
	{
		return nullptr;
	}

	UShopItemSlotWidget* Item = CreateWidget<UShopItemSlotWidget>(GetWorld(), ShopSkillItemSlotClass);
	UOverlaySlot* NewSlot = TargetOverlay->AddChildToOverlay(Item);

	if (NewSlot)
	{
		NewSlot->SetHorizontalAlignment(HAlign_Fill);
		NewSlot->SetVerticalAlignment(VAlign_Fill);
	}

	Item->SetItemData(Icon, Name, Price, Type);

	if (SkillMaxCount != 0)
	{
		Item->SetSkillMaxCount(SkillMaxCount);
		Item->SetSkillCurCount(SkillCurCount);
	}

	if (PlayerBudget < Price || SkillCurCount >= SkillMaxCount )
	{
		Item->SetSlotEnabled(false);
	}
	else
	{
		Item->SetSlotEnabled(true);
	}


	return Item;
}

void UShopRootWidget::ClearAllOverlay()
{
	ClassicOverlay->ClearChildren();
	ShortyOverlay->ClearChildren();
	FrenzyOverlay->ClearChildren();
	GhostOverlay->ClearChildren();
	SheriffOverlay->ClearChildren();

	StingerOverlay->ClearChildren();
	SpectreOverlay->ClearChildren();
	BuckyOverlay->ClearChildren();
	JudgeOverlay->ClearChildren();

	BulldogOverlay->ClearChildren();
	GuardianOverlay->ClearChildren();
	PhantomOverlay->ClearChildren();
	VandalOverlay->ClearChildren();

	MarshalOverlay->ClearChildren();
	OperatorOverlay->ClearChildren();
	AresOverlay->ClearChildren();
	OdinOverlay->ClearChildren();

	LightShieldOverlay->ClearChildren();
	HeavyShieldOverlay->ClearChildren();

	SkillQOverlay->ClearChildren();
	SkillEOverlay->ClearChildren();
	SkillCOverlay->ClearChildren();
}

void UShopRootWidget::IntializeWeaponOverlay()
{
	// SIDEARM
	AddSlotToOverlay(
		ClassicOverlay,
		ClassicIcon,
		TEXT("CALSSIC"),
		29000,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		ShortyOverlay,
		ShortyIcon,
		TEXT("SHORTY"),
		29000,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		FrenzyOverlay,
		FrenzyIcon,
		TEXT("FRENZY"),
		29000,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		GhostOverlay,
		GhostIcon,
		TEXT("Ghost"),
		29000,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		SheriffOverlay,
		SheriffIcon,
		TEXT("SHERIFF"),
		29000,
		EShopItemType::Weapon);

	// SMGS & SHOTGUN
	AddSlotToOverlay(
		StingerOverlay,
		StingerIcon,
		TEXT("STINGER"),
		29000,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		SpectreOverlay,
		SpectreIcon,
		TEXT("SPECTRE"),
		29000,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		BuckyOverlay,
		BuckyIcon,
		TEXT("BUCKY"),
		29000,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		JudgeOverlay,
		JudgeIcon,
		TEXT("JUDGE"),
		29000,
		EShopItemType::Weapon);

	// RIFLES
	AddSlotToOverlay(
		BulldogOverlay,
		BulldogIcon,
		TEXT("BULLDOG"),
		29000,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		GuardianOverlay,
		GuardianIcon,
		TEXT("GUARDIAN"),
		29000,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		PhantomOverlay,
		PhantomIcon,
		TEXT("PHANTOM"),
		29000,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		VandalOverlay,
		VandalIcon,
		TEXT("VANDAL"),
		2900,
		EShopItemType::Weapon);

	// SNIPER & MACHINEGUN
	AddSlotToOverlay(
		MarshalOverlay,
		MarshalIcon,
		TEXT("MARSHAL"),
		29000,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		OperatorOverlay,
		OperatorIcon,
		TEXT("OPERATOR"),
		29000,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		AresOverlay,
		AresIcon,
		TEXT("Ares"),
		29000,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		OdinOverlay,
		OdinIcon,
		TEXT("ODIN"),
		29000,
		EShopItemType::Weapon);
}

void UShopRootWidget::IntializeShiledOverlay()
{
	AddShieldSlotToOverlay(
		LightShieldOverlay,
		LightShieldIcon,
		TEXT("LIGHT SHIELD"),
		400,
		EShopItemType::Shield);

	AddShieldSlotToOverlay(
		HeavyShieldOverlay,
		HeavyShieldIcon,
		TEXT("HEAVY SHIELD"),
		1000,
		EShopItemType::Shield);
}

void UShopRootWidget::IntializeSkillOverlay()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	AValorantPlayer* Player = Cast<AValorantPlayer>(PC->GetPawn());
	if (!Player)
	{
		return;
	}

	USkillComponent* SkillComp = Player->GetSkillComponent();
	PlayerBudget = Player->GetCurrentBudget();

	AddSkillSlotToOverlay(
		SkillQOverlay,
		SkillComp->GetQICon(),
		SkillComp->GetQText(),
		SkillComp->GetQPrice(),
		EShopItemType::Skill,
		SkillComp->GetSkillMaxQCount(),
		SkillComp->GetSkillQCount());

	AddSkillSlotToOverlay(
		SkillEOverlay,
		SkillComp->GetEICon(),
		SkillComp->GetEText(),
		SkillComp->GetEPrice(),
		EShopItemType::Skill,
		SkillComp->GetSkillMaxECount(),
		SkillComp->GetSkillECount());

	AddSkillSlotToOverlay(
		SkillCOverlay,
		SkillComp->GetCICon(),
		SkillComp->GetCText(),
		SkillComp->GetCPrice(),
		EShopItemType::Skill,
		SkillComp->GetSkillMaxCCount(),
		SkillComp->GetSkillCCount());
}
