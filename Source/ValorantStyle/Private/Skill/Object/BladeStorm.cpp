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

}

