// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/PhoenixSkillComponent.h"


void UPhoenixSkillComponent::BeginPlay()
{
	SkillQCount = 1;
	SkillECount = 1;
	SkillCCount = 2;
	NeedUltimateCount = 7;
	CurrentUltimateCount = 0;
}

void UPhoenixSkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
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
