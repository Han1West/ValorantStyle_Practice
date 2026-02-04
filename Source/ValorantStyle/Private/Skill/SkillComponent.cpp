// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/SkillComponent.h"
#include "Player/ValorantPlayer.h"

// Sets default values for this component's properties
USkillComponent::USkillComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USkillComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void USkillComponent::DontHaveSkill()
{
	UE_LOG(LogTemp, Display, TEXT("Don't Have Skill !"));
}

void USkillComponent::NeedMoreSkill()
{
	UE_LOG(LogTemp, Display, TEXT("Need More Ultimate Point !"));
}


// Called every frame
void USkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USkillComponent::ProhibitPlayerSwap()
{
	// 스킬을 사용하는동안 손을 숨기고 스왑을 금지시킴
	OwnerPlayer->SetHideHands(true);
	OwnerPlayer->SetSwapWeapon(false);
}

void USkillComponent::PermitPlayerSwap()
{
	OwnerPlayer->GetWorldTimerManager().ClearTimer(CastingTimerHandle);

	// 손을 다시 보이게하고, 스왑 가능
	OwnerPlayer->SetRevealHands(true);
	OwnerPlayer->SetSwapWeapon(true);
}

void USkillComponent::StartPermitTimer(float Time)
{
	// 기존 타이머 초기화
	OwnerPlayer->GetWorldTimerManager().ClearTimer(CastingTimerHandle);
	// 일정 시간이 지나면 해제
	OwnerPlayer->GetWorldTimerManager().SetTimer(CastingTimerHandle, this, &USkillComponent::PermitPlayerSwap, SkillQCastingTime, false);
}

