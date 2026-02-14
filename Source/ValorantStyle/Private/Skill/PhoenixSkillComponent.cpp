// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/PhoenixSkillComponent.h"
#include "UObject/ConstructorHelpers.h"

UPhoenixSkillComponent::UPhoenixSkillComponent()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> QTextureObj(TEXT("/Game/UIAsset/Phoenix_Q"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> ETextureObj(TEXT("/Game/UIAsset/Phoenix_E"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> CTextureObj(TEXT("/Game/UIAsset/Phoenix_C"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> UltiTextureObj(TEXT("/Game/UIAsset/Phoenix_Ulti"));

	if (QTextureObj.Succeeded())
	{
		QSkillIconTexture = QTextureObj.Object;
	}
	if (ETextureObj.Succeeded())
	{
		ESkillIconTexture = ETextureObj.Object;
	}
	if (CTextureObj.Succeeded())
	{
		CSkillIconTexture = CTextureObj.Object;
	}
	if (UltiTextureObj.Succeeded())
	{
		UltiSkillIconTexture = UltiTextureObj.Object;
	}

	CharacterClass = ECharacterClass::Initiator;
}

void UPhoenixSkillComponent::BeginPlay()
{
	SkillMaxQCount = 1;
	SkillQCount = 1;

	SkillMaxECount = 2;
	SkillECount = 2;

	SkillMaxCCount = 1;
	SkillCCount = 1;

	NeedUltimateCount = 6;
	CurrentUltimateCount = 0;
}

void UPhoenixSkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
}

void UPhoenixSkillComponent::CharacterSelected()
{
	Super::CharacterSelected();
}

void UPhoenixSkillComponent::UseSkillQ()
{
	// 스킬이 남아있지 않으면 사용 불가
	if (SkillQCount <= 0)
	{
		DontHaveSkill();
		return;
	}

	SkillQCount--;

	UE_LOG(LogTemp, Display, TEXT("Use Phoenix Skill Q"));
}

void UPhoenixSkillComponent::UseSkillE()
{
	// 스킬이 남아있지 않으면 사용 불가
	if (SkillECount <= 0)
	{
		DontHaveSkill();
		return;
	}

	SkillECount--;

	UE_LOG(LogTemp, Display, TEXT("Use Phoenix Skill E"));
}

void UPhoenixSkillComponent::UseSkillC()
{
	// 스킬이 남아있지 않으면 사용 불가
	if (SkillCCount <= 0)
	{
		DontHaveSkill();
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("Use Phoenix Skill C"));
}

void UPhoenixSkillComponent::UseSkillUlti()
{
	// 궁극기 포인트가 부족하면 사용 불가
	if (CurrentUltimateCount <= NeedUltimateCount)
	{
		NeedMoreSkill();
		return;
	}

	CurrentUltimateCount = 0;

	UE_LOG(LogTemp, Display, TEXT("Use Phoenix Skill Ulti"));
}
