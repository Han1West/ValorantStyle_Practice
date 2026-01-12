// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/ReloadEnd_Notify.h"
#include "Player/ValorantPlayer.h"

void UReloadEnd_Notify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp)
		return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
		return;

	if (AValorantPlayer* Player = Cast<AValorantPlayer>(Owner))
	{
		Player->EndReload();
	}
}
