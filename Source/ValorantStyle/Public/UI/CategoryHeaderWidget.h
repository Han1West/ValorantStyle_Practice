// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CategoryHeaderWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALORANTSTYLE_API UCategoryHeaderWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CategoryText;

	UFUNCTION(BlueprintCallable)
	void SetTitle(const FString& Title);
};
