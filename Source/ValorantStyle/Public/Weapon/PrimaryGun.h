// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "PrimaryGun.generated.h"

/**
 * 
 */
UCLASS()
class VALORANTSTYLE_API APrimaryGun : public ABaseWeapon
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APrimaryGun();

	virtual void FireOnce() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void StartReload() override;
	virtual void EndReload() override;
	virtual void AllAnimEndReload() override;

	virtual void SetPullTrigger(bool Pull) override;
protected:
	virtual void OnReloadStart() override;

private:
	bool CanFire() const;
	void StartFire();
	void StopFire();
};
