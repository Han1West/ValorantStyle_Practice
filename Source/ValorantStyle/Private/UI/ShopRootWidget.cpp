// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ShopRootWidget.h"
#include "UI/ShopItemSlotWidget.h"
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
	AValorantPlayer* Player = GetOwningPlayerPawn<AValorantPlayer>();
	if (!Player) return;

	Player->OnEquipmentChanged.RemoveDynamic(this, &UShopRootWidget::RefreshAllSlots);
	Player->OnEquipmentChanged.AddDynamic(this, &UShopRootWidget::RefreshAllSlots);


	ClearAllOverlay();

	IntializeSkillOverlay();
	IntializeWeaponOverlay();
	IntializeShiledOverlay();	

	RefreshAllSlots();
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
	Slots.Add(Item);
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
	Slots.Add(Item);
	return Item;
}


UShopItemSlotWidget* UShopRootWidget::AddSkillSlotToOverlay(UOverlay* TargetOverlay, UTexture2D* Icon, const FString& Name, int32 Price, EShopItemType Type, const FString& SkilBaseName, int32 SkillMaxCount, int32 SkillCurCount)
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

	Item->SetSkillItemData(Icon, Name, Price, Type, SkilBaseName);

	if (SkillMaxCount != 0)
	{
		Item->SetSkillMaxCount(SkillMaxCount);
		Item->SetSkillCurCount(SkillCurCount);
	}

	Slots.Add(Item);
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
		0,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		ShortyOverlay,
		ShortyIcon,
		TEXT("SHORTY"),
		300,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		FrenzyOverlay,
		FrenzyIcon,
		TEXT("FRENZY"),
		450,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		GhostOverlay,
		GhostIcon,
		TEXT("Ghost"),
		500,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		SheriffOverlay,
		SheriffIcon,
		TEXT("SHERIFF"),
		800,
		EShopItemType::Weapon);

	// SMGS & SHOTGUN
	AddSlotToOverlay(
		StingerOverlay,
		StingerIcon,
		TEXT("STINGER"),
		1100,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		SpectreOverlay,
		SpectreIcon,
		TEXT("SPECTRE"),
		1600,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		BuckyOverlay,
		BuckyIcon,
		TEXT("BUCKY"),
		850,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		JudgeOverlay,
		JudgeIcon,
		TEXT("JUDGE"),
		1850,
		EShopItemType::Weapon);

	// RIFLES
	AddSlotToOverlay(
		BulldogOverlay,
		BulldogIcon,
		TEXT("BULLDOG"),
		2050,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		GuardianOverlay,
		GuardianIcon,
		TEXT("GUARDIAN"),
		2250,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		PhantomOverlay,
		PhantomIcon,
		TEXT("PHANTOM"),
		2900,
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
		950,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		OperatorOverlay,
		OperatorIcon,
		TEXT("OPERATOR"),
		4700,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		AresOverlay,
		AresIcon,
		TEXT("Ares"),
		1600,
		EShopItemType::Weapon);
	AddSlotToOverlay(
		OdinOverlay,
		OdinIcon,
		TEXT("ODIN"),
		3200,
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
		TEXT("SKILL Q"),
		SkillComp->GetSkillMaxQCount(),
		SkillComp->GetSkillQCount());

	AddSkillSlotToOverlay(
		SkillEOverlay,
		SkillComp->GetEICon(),
		SkillComp->GetEText(),
		SkillComp->GetEPrice(),
		EShopItemType::Skill,
		TEXT("SKILL E"),
		SkillComp->GetSkillMaxECount(),
		SkillComp->GetSkillECount());

	AddSkillSlotToOverlay(
		SkillCOverlay,
		SkillComp->GetCICon(),
		SkillComp->GetCText(),
		SkillComp->GetCPrice(),
		EShopItemType::Skill,
		TEXT("SKILL C"),
		SkillComp->GetSkillMaxCCount(),
		SkillComp->GetSkillCCount());
}

void UShopRootWidget::RefreshAllSlots()
{
	for (UShopItemSlotWidget* CurSlot : Slots)
	{
		CurSlot->RefreshState();
	}
}
