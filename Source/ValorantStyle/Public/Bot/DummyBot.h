// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DummyBot.generated.h"

UENUM(BlueprintType)
enum class EHitPart : uint8
{
	Body,
	Head,	
	ArmLeg,

	None
};

UCLASS()
class VALORANTSTYLE_API ADummyBot : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADummyBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UFUNCTION(BlueprintPure)
	float GetHPPercent() const;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

private:
	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100;
	UPROPERTY(EditDefaultsOnly)
	float MaxShield = 50;
	UPROPERTY(VisibleAnywhere)
	float Health = 0;
	UPROPERTY(VisibleAnywhere)
	float Shield = 0;	
	UPROPERTY(EditAnywhere)
	bool bUseShield = false;

	UPROPERTY()
	class ABotSpawner* BotSpawner = nullptr;


	FTimerHandle RespawnTimerHandle;

	TMap<FName, EHitPart> BoneToPartMap;
private:
	void InitBoneMap();

	float CalculatePartialDamage(float OriginDamage, FDamageEvent const& DamageEvent);
	void CalculateDamageWithShield(float OriginDamage);
	void DummyDeathProcess();
	void DummyRespawnWithNewLocation();

public:
	void SetUseShield(bool bUse) { bUseShield = bUse; }
	void OnShield(bool bMax);
	void OffShield();
};
