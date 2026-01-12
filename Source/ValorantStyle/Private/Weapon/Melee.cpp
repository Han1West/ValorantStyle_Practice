// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Melee.h"
#include "Player/ValorantPlayer.h"

// Sets default values
AMelee::AMelee()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMelee::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AValorantPlayer* Player = Cast<AValorantPlayer>(GetOwner());

	if (Player)
	{		
		bool bInspectNow = Player->IsInspectWeapon();
		
		// Off -> On
		if(!bPrevInspect && bInspectNow)
		{
			SetActorRelativeLocation(FVector(0.f, -4.f, -6.f));
			SetActorRelativeRotation(FRotator(38.f, 35.f, 30.f));			
		}

		// On -> Off
		if (bPrevInspect && !bInspectNow)
		{
			SetActorRelativeLocation(FVector(0.f, 0.f, 0.f));
			SetActorRelativeRotation(FRotator(0.f, 0.f, 0.f)); 			
		}

		bPrevInspect = bInspectNow;
	}
}


