// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ValorantPlayer.generated.h"

enum class ECharacterType
{
	Jett,
	Phoenix,
	NONE,
};

class ABaseWeapon;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnSkillComponentChanged,
	USkillComponent*, NewSkillComponent
);

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

	UFUNCTION(BlueprintPure)
	float GetCurrentHealth() const;
	UFUNCTION(BlueprintPure)
	float GetCurrentShiled() const;
	UFUNCTION(BlueprintPure)
	float GetCurrentShiledPercent() const;
	UFUNCTION(BlueprintPure)
	ABaseWeapon* GetCurrentWeapon() const;



	class UCameraComponent* GetCameraComponent() const { return CameraComp; }

	class USkillComponent* GetSkillComponent() const { return SkillComponent; }

	void RequestHideHands(bool ChangeWeapon);
	void RequestRevealHands(bool ChangeWeapon); 
	void SetCanSwapWeapon(bool Swap) { bCanSwapWeapon = Swap; }
	void SetTemporaryWeaponIdx(int32 Idx) { TemporaryWeaponIdx = Idx; }

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

	//
	UPROPERTY(BlueprintAssignable)
	FOnSkillComponentChanged OnSkillComponentChanged;

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
	void DummyNormalShieldTriggerPressed();
	void DummyMaxShieldTriggerPressed();

	void SkillQPressed();
	void SkillQReleased();
	void SkillEPressed();
	void SkillEReleased();
	void SkillCPressed();
	void SkillCReleased();
	void SkillUltiPressed();
	void SkillUltiReleased();

	void UseWeaponSurAbility();
	void ReleaseWeaponSurAbility();

	void UseWeaponSubAbility();
	void ReleaseWeaponSubAbility();

	void SetCharaterTypeToJett();
	void SetCharaterTypeToPhoenix();

	void SpawnBladestorm();
	void DeSpawnBladeStorm();
	void HideHands(bool bChgangeWeapon);
	void RevealHands(bool bChangeWeapon);
	//

	void EquipWeapon(int32 Index);
	void TakeReload();		

	void SelectWeapon0(); 
	void SelectWeapon1(); 

	void ApplyRecoil(float DeltaTime);

	void SetCharacterType(ECharacterType Type);
	void CheckUsePassive();

	FTransform GetBladeTransformForView(int Index);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPS", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPS", meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* ArmsMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FPS", meta = (AllowPrivateAccess = "true"))
	class USceneComponent* FPView;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkillComponent* SkillComponent;

	UPROPERTY(VisibleAnywhere)
	class UJettSkillComponent* JettSkill;
	UPROPERTY(VisibleAnywhere)
	class UPhoenixSkillComponent* PhoenixSkill;

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

	bool bHideHands = false;
	bool bRevealHands = false;
	bool bInvisibleHands = false;
	bool bCanSwapWeapon = true;
	bool bChangeWeaponWhenHideOrReveal = false;

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

	ECharacterType CurrentCharacterType = ECharacterType::NONE;
private:
	float OriginMaxSpeed = 0.f;
	int32 CurrentWeaponIdx = -1;
	int32 TemporaryWeaponIdx = 0;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100;
	UPROPERTY(EditDefaultsOnly)
	float MaxShield = 50;
	UPROPERTY(VisibleAnywhere)
	float Health = 85;
	UPROPERTY(VisibleAnywhere)
	float Shield = 50;

	class ABotSpawner* BotSpawner = nullptr;
};
