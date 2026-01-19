// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DummyBot.generated.h"

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
	float Health;
	UPROPERTY(VisibleAnywhere)
	float Shield;	
};
