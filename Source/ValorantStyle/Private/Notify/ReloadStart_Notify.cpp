// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/ReloadStart_Notify.h"
#include "Player/ValorantPlayer.h"

void UReloadStart_Notify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp)
		return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
		return;

	if (AValorantPlayer* Player = Cast<AValorantPlayer>(Owner))
	{
		Player->StartReload();
	}
}