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

	if(bPassive && OwnerPlayer)
	{
		UseSkillPassive();
	}

	// ...
}

void USkillComponent::CharacterSelected()
{
	// 스킬 아이콘 변경
	BroadcastAllIcons();
}

float USkillComponent::GetSkillDurationPercent() const
{
	if (bActiveHasDurationSkill)
	{
		return (1.f - (CurrentActiveAccTime / CurrentActiveDurationTime));
	}
	else
	{
		return 0.f;
	}
		
}

void USkillComponent::BroadcastAllIcons()
{
	UE_LOG(LogTemp, Warning, TEXT("Broadcast from %s"), *GetName());

	if (QSkillIconTexture)
	{
		UE_LOG(LogTemp, Warning, TEXT("Icon Valid: %s"), *QSkillIconTexture->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Icon NULL"));
	}
	OnSkillIconChanged.Broadcast(ESkillSlot::Q, QSkillIconTexture);
	OnSkillIconChanged.Broadcast(ESkillSlot::E, ESkillIconTexture);
	OnSkillIconChanged.Broadcast(ESkillSlot::C, CSkillIconTexture);
	OnSkillIconChanged.Broadcast(ESkillSlot::Ulti, UltiSkillIconTexture);
}

void USkillComponent::GetKill()
{
	// 킬로 Ultimate 충전
	if (CurrentUltimateCount < NeedUltimateCount)
	{
		CurrentUltimateCount++;
	}

	// 킬로 E스킬 충전
	if (CharacterClass == ECharacterClass::Duelist)
	{		
		// 현재 E스킬이 소비된 상황
		if (SkillECount < SkillMaxECount)
		{			
			KillCount++;			

			// 킬카운트를 더해서 조건이 충족되면 E스킬 충전
			if (KillCount == 2)
			{
				SkillECount++;
				KillCount = 0;
			}
		}
	}
}

void USkillComponent::ProhibitPlayerSwap()
{
	// 스킬을 사용하는동안 손을 숨기고 스왑을 금지시킴
	OwnerPlayer->RequestHideHands(false);
	bDeactivatedPlayerHands = true;
}

void USkillComponent::PermitPlayerSwap()
{
	// 손을 다시 보이게하고, 스왑 가능
	OwnerPlayer->RequestRevealHands(false);
	bDeactivatedPlayerHands = false;
}

void USkillComponent::CastingEndQSkil()
{
	OwnerPlayer->GetWorldTimerManager().ClearTimer(CastingQHandle);
	bSkillQCasting = false;
	OffUsingSkill();
}

void USkillComponent::CastingEndESkil()
{
	OwnerPlayer->GetWorldTimerManager().ClearTimer(CastingEHandle);
	bSkillECasting = false;
	OffUsingSkill();
}

void USkillComponent::CastingEndCSkil()
{
	OwnerPlayer->GetWorldTimerManager().ClearTimer(CastingCHandle);
	bSkillCCasting = false;
	OffUsingSkill();
}

void USkillComponent::CastingEndUltiSkil()
{
	OwnerPlayer->GetWorldTimerManager().ClearTimer(CastingUltiHandle);
	bSkillUltiCasting = false;
	OffUsingSkill();
}

void USkillComponent::OffUsingSkill()
{
	// 모든 일반스킬이 꺼져있으면 
	if (!bSkillCCasting && !bSkillECasting && !bSkillQCasting)
	{
		bUsingSkill = false;
	}
}