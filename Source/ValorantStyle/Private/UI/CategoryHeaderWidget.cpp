// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CategoryHeaderWidget.h"
#include "Components/TextBlock.h"

void UCategoryHeaderWidget::SetTitle(const FString& Title)
{
	if (CategoryText)
	{
		CategoryText->SetText(FText::FromString(Title));
	}
}