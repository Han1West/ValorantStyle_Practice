// Fill out your copyright notice in the Description page of Project Settings.


#include "Bot/BotSpawner.h"
#include "Bot/DummyBot.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"

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

	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	SpawnArea->SetupAttachment(CollisionBox);

}

// Called when the game starts or when spawned
void ABotSpawner::BeginPlay()
{
	Super::BeginPlay();

	// 더미들을 미리 스폰하고 숨긴다.
	for (int i = 0; i < MaxSpawnCount; ++i)
	{
		SpawnedBot = GetWorld()->SpawnActor<ADummyBot>(DummyBotClass);
		SpawnedBot->SetActorHiddenInGame(true);
		SpawnedBot->SetActorEnableCollision(false);
		SpawnedBotArray.Add(SpawnedBot);
	}

	AdjustZLocaiton = SpawnedBot->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
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
	if (bActivate)
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
		TrySpawnAllBot();
	}
	else
	{
		DespawnAllBot();
	}

	// 일정시간 동안 반복된 Toggle이 들어오지 못하게 한다.
	bActivate = false;
}

void ABotSpawner::TrySpawnAllBot()
{
	UE_LOG(LogTemp, Warning, TEXT("Spawned!!"));

	int32 TryCount = 0;
	int32 MaxTry = 100;

	// 미리 스폰해놓은 더미들의 위치를 랜덤으로 배치하고 활성화 시킨다
	while (CurSpawnedCount < MaxSpawnCount && TryCount < MaxTry)
	{
		TryCount++;

		ADummyBot* NonSpawnedBot = nullptr;
		// 아직 스폰이 안된 Bot
		for (ADummyBot* Bot : SpawnedBotArray)
		{
			if (Bot->IsHidden())
			{
				NonSpawnedBot = Bot;
				break;
			}
		}

		// 더이상 스폰 안된 Bot이 없으면
		if (!NonSpawnedBot)
		{
			return;
		}


		FVector SpawnLocation;

		if (!GetGroundPointInBox(SpawnLocation))
		{
			continue;
		}

		NonSpawnedBot->SetActorLocation(SpawnLocation);
		NonSpawnedBot->SetActorHiddenInGame(false);
		NonSpawnedBot->SetActorEnableCollision(true);
		CurSpawnedCount++;

	}
}

void ABotSpawner::DespawnAllBot()
{
	UE_LOG(LogTemp, Warning, TEXT("De Spawned!!"));

	for (ADummyBot* Bot : SpawnedBotArray)
	{
		if (!Bot->IsHidden())
		{
			Bot->SetActorHiddenInGame(true);
			Bot->SetActorEnableCollision(false);
		}
	}

	CurSpawnedCount = 0;
}

bool ABotSpawner::GetGroundPointInBox(FVector& OutLocation) const
{
	// SpawnArea의 위치와 Extent 값
	FVector Location = SpawnArea->GetComponentLocation();
	FVector Extent = SpawnArea->GetScaledBoxExtent();

	// X, Y 에서 랜덤한 위치 값
	float MinX = Location.X - Extent.X;
	float MaxX = Location.X + Extent.X;
	float RandomX = FMath::FRandRange(Location.X - Extent.X, Location.X + Extent.X);
	float Alpha = (RandomX - MinX) / (MaxX - MinX);
	Alpha = FMath::Clamp(Alpha, 0.f, 1.f);

	float MinWidthRatio = 0.3f;
	float CurrentYExtent = FMath::Lerp(Extent.Y * MinWidthRatio, Extent.Y, Alpha);
	float RandomY = FMath::FRandRange(Location.Y - CurrentYExtent, Location.Y + CurrentYExtent);

	float StartZ = Location.Z + Extent.Z;
	float EndZ = Location.Z - Extent.Z;

	FVector Start(RandomX, RandomY, StartZ);
	FVector End(RandomX, RandomY, EndZ);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	if (bHit)
	{
		OutLocation = Hit.ImpactPoint + AdjustZLocaiton;
		return true;
	}
	return false;
}
