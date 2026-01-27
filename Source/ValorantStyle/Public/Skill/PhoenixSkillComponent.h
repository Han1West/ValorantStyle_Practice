// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill/SkillComponent.h"
#include "PhoenixSkillComponent.generated.h"

/**
 * 
 */
UCLASS()
class VALORANTSTYLE_API UPhoenixSkillComponent : public USkillComponent
{
	GENERATED_BODY()

public:
	virtual void UseSkill1() override;
	virtual void UseSkill2() override;
	virtual void UseSkill3() override;
	virtual void UseSkillUlti() override;
};
