// Fill out your copyright notice in the Description page of Project Settings.


#include "Bot/BotSpawner.h"
#include "Components/BoxComponent.h"

// Sets default values
ABotSpawner::ABotSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;

	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	CollisionBox->SetGenerateOverlapEvents(false);
}

// Called when the game starts or when spawned
void ABotSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABotSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (!bActivate)
	{
		DelayTime += DeltaTime;

		if (DelayTime > 1.f)
		{
			DelayTime = 0.f;
			bActivate = true;
		}
	}

}

float ABotSpawner::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Display, TEXT("BotSpanwer is Hitted"));
	UE_LOG(LogTemp, Display, TEXT("%s"), bActivate ? TEXT("true") : TEXT("false"));

	// 스폰 토글
	if(bActivate)
	{
		ToggleSpawn();		
	}

	return 0;
}

void ABotSpawner::ToggleSpawn()
{
	bBotSpawned = !bBotSpawned;

	if (bBotSpawned)
	{
		SpawnBot();
	}
	else
	{
		DespawnBot();
	}

	// 일정시간 동안 반복된 Toggle이 들어오지 못하게 한다.
	bActivate = false;
}

void ABotSpawner::SpawnBot()
{
	UE_LOG(LogTemp, Warning, TEXT("Spanwed!!"));
}

void ABotSpawner::DespawnBot()
{
	UE_LOG(LogTemp, Warning, TEXT("De Spanwed!!"));
}
