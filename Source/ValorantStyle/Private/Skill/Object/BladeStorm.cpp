// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/Object/BladeStorm.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ABladeStorm::ABladeStorm()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BladeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BladeMesh"));
	RootComponent = BladeMesh;

	ProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	ProjectileComponent->UpdatedComponent = BladeMesh;
	

	SetActorScale3D(FVector(0.1f, 0.1f, 0.1f));
}

// Called when the game starts or when spawned
void ABladeStorm::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABladeStorm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bAttached)
	{
		AccTime += DeltaTime;

		FVector Shake;
		Shake.X = FMath::Sin(AccTime * ShakeSpeed) * ShakeStrength;
		Shake.Y = FMath::Cos(AccTime * ShakeSpeed * 1.3f) * ShakeStrength;
		Shake.Z = FMath::Sin(AccTime * ShakeSpeed * 0.7f) * ShakeStrength;

		BladeMesh->SetRelativeLocation(InitialRelativeLocation + Shake);
	}
}

void ABladeStorm::SetInitialRelativeLocation()
{
	InitialRelativeLocation = BladeMesh->GetRelativeLocation();
}

