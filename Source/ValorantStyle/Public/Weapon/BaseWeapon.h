// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Idle,
	Firing,
	Reloading,

	NONE,
};

UCLASS()
class VALORANTSTYLE_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool TryReload();
	virtual void SetPullTrigger(bool Pull);

	virtual void StartReload();
	virtual void EndReload();
	virtual void AllAnimEndReload();

	EWeaponState GetCurWeaponState() const;
public:
	FVector2D CurrentRecoilOffset = FVector2D::ZeroVector;
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	float RecoilRiseSpeed = 25.f;
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	float RecoilRecoverSpeed = 30.f;

	float SpreadRecoverSpeed = 30.f;

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;
protected:
	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditDefaultsOnly, Category = "Magazine")
	TSubclassOf<class AMagazine> MagazineClass;
	
	UPROPERTY()
	AMagazine* CurrentMagazine;

	UPROPERTY(VisibleAnywhere)
	int32 maxAmmo = 0;
	UPROPERTY(VisibleAnywhere)
	int32 leftAmmo = 0;
	UPROPERTY(VisibleAnywhere)
	int32 curAmmo = 0;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float TriggerDelayTime = 0.f;
	FTimerHandle FireTimerHandle;

	EWeaponState WeaponState = EWeaponState::Idle;
	bool bPullTrigger = false;	
	
	int32 CurrentShotIndex = 0;
	float LastFireTime = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	float RecoilResetTime = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	TArray<FVector2D> SprayPattern;
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	float BaseRecoilMultiplier = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	float MoveRecoilMultiplier = 1.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	float CrouchRecoilMultiplier = 0.7f;

	UPROPERTY(EditDefaultsOnly, Category = "Accuracy")
	float BaseSpread = 0.f;
	UPROPERTY(EditDefaultsOnly, Category = "Accuracy")
	float MoveSpread = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Accuracy")
	float FireSpread = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Accuracy")
	float MaxSpread = 50.f;
	float CurrentSpread = 0.f;

	float RandomSpread = 0.f;
	float PatternSpread = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "Recoil")
	FVector2D AccumulatedAimOffset;

	UPROPERTY(EditAnywhere)
	float MaxRange = 1000.f;
	UPROPERTY(EditAnywhere)
	float Damage = 10.f;

protected:
	virtual bool CanReload() const;
	virtual void OnReloadStart();

	virtual void FireOnce();

	bool GunTraceWithRecoil(FHitResult& Hit, FVector& ShotDirection, const FVector2D& Recoil);

	FVector2D GetCurrentRecoil() const;

	float GetRecoilMultiplier() const;
	float GetCurrentSpread() const;

	bool IsUnstableState() const;

	AController* GetOwnerController() const;
};
