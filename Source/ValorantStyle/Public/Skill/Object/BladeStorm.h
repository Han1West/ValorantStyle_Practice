// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BladeStorm.generated.h"

UCLASS()
class VALORANTSTYLE_API ABladeStorm : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABladeStorm();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetAttached(bool Attached) { bAttached = Attached; }
	void SetInitialRelativeLocation();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent * BladeMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileComponent;

	FVector InitialRelativeLocation;
	float AccTime;

	UPROPERTY(EditAnywhere, Category = "VFX")
	float ShakeStrength = 0.4f;
	UPROPERTY(EditAnywhere, Category = "VFX")
	float ShakeSpeed = 6.f;
	UPROPERTY(EditAnywhere, Category = "VFX")
	float RotationShake = 2.f;

	float Damage = 50.f;

	bool bAttached = false;
};
