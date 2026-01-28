// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillComponent.generated.h"

class AValorantPlayer;

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

	void DontHaveSkill();
	void NeedMoreSkill();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	virtual void UseSkillQ() {};
	virtual void UseSkillE() {};
	virtual void UseSkillC() {};
	virtual void UseSkillUlti() {};
	virtual void UseSkillPassive() {};

	void SetOwnerPlayer(AValorantPlayer* Player) { OwnerPlayer = Player; }
	bool IsHavePassiveSkill() { return bPassive; }

protected:
	AValorantPlayer* OwnerPlayer = nullptr;

	bool bPassive = false;

	int SkillQCount = 0;
	int SkillECount = 0;
	int SkillCCount = 0;
	int NeedUltimateCount = 0;
	int CurrentUltimateCount = 0;
};
