// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillComponent.generated.h"

class AValorantPlayer;

UENUM(BlueprintType)
enum class ESkillSlot : uint8
{
	Q,
	E,
	C,
	Ulti,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnSkillIconChanged,
	ESkillSlot, Slot,
	UTexture2D*, NewIcon
);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VALORANTSTYLE_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USkillComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 캐릭터가 선택되고 미리 수행할 작업들
	virtual void CharacterSelected();
	
	virtual void UseSkillQ() {};
	virtual void ReleaseSKillQ() {};	

	virtual void UseSkillE() {};
	virtual void ReleaseSKillE() {};

	virtual void UseSkillC() {};
	virtual void ReleaseSKillC() {};

	virtual void UseSkillUlti() {};
	virtual void ReleaseSKillUlti() {};

	virtual void UseSkillPassive() {};

	virtual void PressMouseLBTN() {};
	virtual void PressMouseRBTN() {};
	virtual void ReleaseMouseLBTN() {};
	virtual void ReleaseMouseRBTN() {};

	virtual void CheckPlayerKeyInput(FKey PressedKey) {};

	void SetOwnerPlayer(AValorantPlayer* Player) { OwnerPlayer = Player; }
	bool IsHavePassiveSkill() { return bPassive; }
	
	UFUNCTION(BlueprintPure)
	UTexture2D* GetQICon() const { return QSkillIconTexture; }
	UFUNCTION(BlueprintPure)
	UTexture2D* GetEICon() const { return ESkillIconTexture; }
	UFUNCTION(BlueprintPure)
	UTexture2D* GetCICon() const { return CSkillIconTexture; }
	UFUNCTION(BlueprintPure)
	UTexture2D* GetUltiICon() const { return UltiSkillIconTexture; }

	UFUNCTION(BlueprintPure)
	int32 GetSkillMaxQCount() const { return SkillMaxQCount; }
	UFUNCTION(BlueprintPure)
	int32 GetSkillQCount() const { return SkillQCount; }

	UFUNCTION(BlueprintPure)
	int32 GetSkillMaxECount() const { return SkillMaxECount; }
	UFUNCTION(BlueprintPure)
	int32 GetSkillECount() const { return SkillECount; }

	UFUNCTION(BlueprintPure)
	int32 GetSkillMaxCCount() const { return SkillMaxCCount; }
	UFUNCTION(BlueprintPure)
	int32 GetSkillCCount() const { return SkillCCount; }

	UFUNCTION(BlueprintPure)
	int32 GetSkillNeedUltiCount() const { return NeedUltimateCount; }
	UFUNCTION(BlueprintPure)
	int32 GetSkillCurrentUltiCount() const { return CurrentUltimateCount; }

	UFUNCTION(BlueprintPure)
	bool IsActiveHasDurationSkill() const { return bActiveHasDurationSkill; }
	UFUNCTION(BlueprintPure)
	float GetSkillDurationPercent() const;


	UPROPERTY(BlueprintAssignable)
	FOnSkillIconChanged OnSkillIconChanged;

	UFUNCTION(BlueprintCallable)
	void BroadcastAllIcons();

protected:
	UPROPERTY()
	AValorantPlayer* OwnerPlayer = nullptr;

	UPROPERTY()
	UTexture2D* QSkillIconTexture;
	UPROPERTY()
	UTexture2D* ESkillIconTexture;
	UPROPERTY()
	UTexture2D* CSkillIconTexture;
	UPROPERTY()
	UTexture2D* UltiSkillIconTexture;

	bool bPassive = false;

	int SkillMaxQCount = 0;
	int SkillQCount = 0;

	int SkillMaxECount = 0;
	int SkillECount = 0;

	int SkillMaxCCount = 0;
	int SkillCCount = 0;

	int NeedUltimateCount = 0;
	int CurrentUltimateCount = 0;

	float SkillQCastingTime = 0.f;
	float SkillECastingTime = 0.f;
	float SkillCCastingTime = 0.f;
	float SkillUltiCastingTime = 0.f;

	float CurrentActiveDurationTime = 0.f;
	float CurrentActiveAccTime = 0.f;
	
	float SkillQDurationmTime = 0.f;
	float SkillEDurationTime = 0.f;
	float SkillCDurationTime = 0.f;
	float SkillUltiDurationTime = 0.f;
	
	bool bActiveHasDurationSkill = false;

	bool bSkillQInput = false;
	bool bSkillQCasting = false;

	bool bSkillEInput = false;
	bool bSkillECasting = false;
	
	bool bSkillCInput = false;
	bool bSkillCCasting = false;

	bool bSkillUltiInput = false;
	bool bSkillUltiCasting = false;

	bool bUsingSkill = false;
	bool bDeactivatedPlayerHands = false;

	FTimerHandle CastingQHandle;
	FTimerHandle CastingEHandle;
	FTimerHandle CastingCHandle;
	FTimerHandle CastingUltiHandle;

protected:
	void DontHaveSkill();
	void NeedMoreSkill();

	void ProhibitPlayerSwap();
	void PermitPlayerSwap();	
	void OffUsingSkill();

	virtual void EndSKillQ() {};
	virtual void EndSKillE() {};
	virtual void EndSKillC() {};
	virtual void EndSkillUlti() {};

public:
	void CastingEndQSkil();
	void CastingEndESkil();
	void CastingEndCSkil();
	void CastingEndUltiSkil();
};
