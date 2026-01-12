// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ValorantPlayer.generated.h"

class ABaseWeapon;

UCLASS()
class VALORANTSTYLE_API AValorantPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AValorantPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	//
	// GET SET
	//
	UFUNCTION(BlueprintPure)
	bool IsInspectWeapon() const;

	UFUNCTION(BlueprintCallable)
	void SetInspectWeapon(bool bInspect);

	UFUNCTION(BlueprintPure)
	bool IsUseSurWeaponAbility() const;

	UFUNCTION(BlueprintPure)
	bool IsUseSubWeaponAbility() const;

	UFUNCTION(BlueprintPure)
	bool IsSwapWeapon() const;

	UFUNCTION(BlueprintCallable)
	void SetSwapWeapon(bool bSwap);

	UFUNCTION(BlueprintPure)
	int32 GetCurWeaponIdx() const;

	UFUNCTION(BlueprintPure)
	enum EWeaponState GetCurWeaponState() const;

	bool IsMoving() { return bMove; }
	bool IsCrouched() { return bCrouch; }
	bool IsAirborne();
	// GET SET
	// 


	// With Notify
	void StartReload();
	void EndReload();
	void AllAnimEndReload();
	void EndDraw();

private:
	void AdjustSpeed();
	void AdjustFPView();
	void TurnOffAllCondition();

	// Key Event
	void MoveFoward(float AxisValue);
	void MoveRight(float AxisValue);
	
	void JumpPressed();
	void WalkPressed();
	void WalkReleased();
	void CrouchPressed();
	void CrouchReleased();
	void InspectPressed();
	void ReloadPressed();

	void UseWeaponSurAbility();
	void ReleaseWeaponSurAbility();

	void UseWeaponSubAbility();
	void ReleaseWeaponSubAbility();
	//

	void EquipWeapon(int32 Index);
	void TakeReload();		

	void SelectWeapon0() { EquipWeapon(0); }
	void SelectWeapon1() { EquipWeapon(1); }

	void ApplyRecoil(float DeltaTime);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPS", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPS", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* ArmsMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPS", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* FPView;

	UPROPERTY(EditAnywhere)
	float Speed = 1.f;
	UPROPERTY(VisibleAnywhere)
	bool bWalk = false;
	UPROPERTY(VisibleAnywhere)
	bool bCrouch = false;
	UPROPERTY(VisibleAnywhere)
	bool bMove = false;
	UPROPERTY(VisibleAnywhere)
	bool bSurWeapon = false;
	UPROPERTY(VisibleAnywhere)
	bool bSubWeapon = false;
	UPROPERTY(VisibleAnywhere)
	bool bInspectWeapon = false;
	UPROPERTY(VisibleAnywhere)
	bool bSwapWeapon = false;
	UPROPERTY(VisibleAnywhere)
	bool bTakeReload = false;


	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TArray<TSubclassOf<ABaseWeapon>> WeaponClasses;
	UPROPERTY()
	TArray<ABaseWeapon*> Weapons;
	UPROPERTY()
	ABaseWeapon* CurrentWeapon;	
	
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* ReloadMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Magazine")
	TSubclassOf<class AMagazine> MagazineClass;
	UPROPERTY()
	AMagazine* CurrentMagazine;

	FVector2D CurrentCameraRecoil = FVector2D::ZeroVector;
	FVector2D LastAppliedRecoil = FVector2D::ZeroVector;

private:
	float OriginMaxSpeed = 0.f;
	int32 CurrentWeaponIdx = -1;
};
