// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "ReloadAllAnimEnd_Notify.generated.h"

/**
 * 
 */
UCLASS()
class VALORANTSTYLE_API UReloadAllAnimEnd_Notify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
