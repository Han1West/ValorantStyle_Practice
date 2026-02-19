// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cloudburst.generated.h"

UCLASS()
class VALORANTSTYLE_API ACloudburst : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACloudburst();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetHomingDirection(FVector Direction);
	void FinishControl();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* MeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* BurstSound;

	bool bActivated = false;
	bool bIsControlled = true;
	float AccTime = 0.f;
	float LifeTime = 6.f;
};
