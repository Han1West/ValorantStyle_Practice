// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BotSpawner.generated.h"

UCLASS()
class VALORANTSTYLE_API ABotSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABotSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawner", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* SpawnArea;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner")
	TSubclassOf<class ADummyBot> DummyBotClass;

	UPROPERTY()
	ADummyBot* SpawnedBot;

	UPROPERTY()
	TArray<ADummyBot*> SpawnedBotArray;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxSpawnCount = 20;
	
	UPROPERTY(EditDefaultsOnly)
	int32 CurSpawnedCount = 0;

	UPROPERTY(VisibleAnywhere)
	bool bBotSpawned = false;

	bool bActivate = true;
	float DelayTime = 0.f;
	float AdjustZLocaiton = 0.f;
private:
	void ToggleSpawn();

	void TrySpawnAllBot();
	void DespawnAllBot();

	bool GetGroundPointInBox(FVector& OutLocation) const;
};
