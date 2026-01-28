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
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	virtual void UseSkillQ() override;
	virtual void UseSkillE() override;
	virtual void UseSkillC() override;
	virtual void UseSkillUlti () override;
	virtual void UseSkillPassive() override;


private:
	bool bTailWind = false;

	float AccTime = 0.f;
};
