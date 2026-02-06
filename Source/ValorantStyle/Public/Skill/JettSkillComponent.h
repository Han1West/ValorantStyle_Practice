// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill/SkillComponent.h"
#include "JettSkillComponent.generated.h"

/**
 * 
 */
UCLASS()
class VALORANTSTYLE_API UJettSkillComponent : public USkillComponent
{
	GENERATED_BODY()
public:
	// Sets default values for this component's properties
	UJettSkillComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	virtual void CharacterSelected() override;

	virtual void UseSkillQ() override;
	virtual void UseSkillE() override;
	virtual void UseSkillC() override;
	virtual void ReleaseSKillC() override;

	virtual void UseSkillUlti () override;
	virtual void UseSkillPassive() override;

	virtual void PressMouseLBTN() override;
	virtual void PressMouseRBTN() override;
	virtual void ReleaseMouseLBTN() override;
	virtual void ReleaseMouseRBTN() override;

	virtual void CheckPlayerKeyInput(FKey PressedKey) override;

	void OnBladeKillSuccess();

private:
	bool bTailWind = false;
	
	UPROPERTY()
	class ACloudburst* ControlledCloudburst = nullptr;

	bool bHoldingC = false;
	bool bWasUsingUlti = false;

	float AccTime = 0.f;
	float ControlledTime = 0.f;
	float BladeStormFireDelay = 0.2f;

	UPROPERTY()
	TSubclassOf<ACloudburst> CloudburstClass;
	UPROPERTY()
	TSubclassOf<class ABladeStorm> BladestormClass;
	UPROPERTY()
	TArray<ABladeStorm*> Bladestorms;

	FTimerHandle FireTimerHandle;
	int32 ActivatedBladestormsCount = 0;

private:
	void UpdateCloudburstDirection();
	void FireBladeStormOnce();
	void FireBladeStormAll();
	void SpawnProjectileBladestorm(const FVector& FireDirection, const FVector& SpawnLocation, const FRotator& SpanwRotation, bool SingleFire);

	FTransform GetBladeTransformForView(int Index);
	void InitialSpawnBladestorm();
	void SpawnBladestorm();
	void RespawnBladestormAfterUsingSkill();
	void DeSpawnBladeStorm();
	void DeSpawnOneBladeStorm();

protected:
	virtual void EndSkillUlti() override;
};
