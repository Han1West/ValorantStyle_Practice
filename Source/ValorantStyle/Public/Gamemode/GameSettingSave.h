// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameSettingSave.generated.h"

/**
 * 
 */
UCLASS()
class VALORANTSTYLE_API UGameSettingSave : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	float MouseSensitivity = 1.f;
	UPROPERTY()
	float MasterVolume = 1.f;
	
};
