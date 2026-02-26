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
enum class EShopItemType : uint8;

struct PlayerEquipment
{
	// 무기
	FString EquippedPrimary = TEXT("NONE");
	ABaseWeapon* Primary = nullptr;
	int32 PrimaryPrice = 0;

	// 쉴드
	FString EquippedShield = TEXT("NONE");
	int32 MaxShield = 0;
	int32 CurrentShield = 0;
	int32 ShieldPrice = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnSkillComponentChanged,
	USkillComponent*, NewSkillComponent
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipmentChanged);

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
	virtual void Landed(const FHitResult& Hit) override;

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
	UFUNCTION(BlueprintPure)
	int32 GetCurrentBudget() { return CurrentBudget; }

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

	UPROPERTY(BlueprintAssignable)
	FOnEquipmentChanged OnEquipmentChanged;

	void PlayerGetKill();
	void SetHandMeshRelativeLocationRotaiton(const FVector& Location, const FRotator& Rotation);
	void BuyItem(const FString& ItemName, int32 Price);
	void ResellItem(const FString& ItemName, int32 Price);

	// 단순히 현재 아이템을 착용중인지
	bool IsItemEquipped(const FString& ItemName, EShopItemType ItemType);
	// 아이템이 착용중이고 팔 수 있는 상태인지
	bool IsItemCanSelled(const FString& ItemName, EShopItemType ItemType);
	

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

	//
	void HideHands(bool bChgangeWeapon);
	void RevealHands(bool bChangeWeapon);
	//

	void EquipWeapon(int32 Index);
	void TakeReload();		
	void StopReload();

	void SelectWeapon0(); 
	void SelectWeapon1(); 

	void ApplyRecoil(float DeltaTime);

	void SetCharacterType(ECharacterType Type);

	void PlayFootstep();
	void StopFootstep();
	
	void SubShield();

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

	UPROPERTY()
	UAudioComponent* FootstepAudio;

	UPROPERTY(EditDefaultsOnly, Category="Sound")
	USoundBase* FootstepSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* JumpStartSound;
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* JumplandingSound;

	float AccumulatedMoveTime = 0.f;
	float AccumulatedStopTime = 0.f;
	float StartThreshold = 0.5f;
	float StopThreshold = 0.2f;

	bool bIsFootstepPlaying = false;

	FVector AdjustLocation;
	FRotator AdjustRotation;
		
	PlayerEquipment CurrentPlayerEquipment;	
	PlayerEquipment PrevPlayerEquipment;

private:
	float OriginMaxSpeed = 0.f;
	int32 CurrentWeaponIdx = -1;
	int32 TemporaryWeaponIdx = 0;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100;
	UPROPERTY(VisibleAnywhere)
	float Health = 100;

	UPROPERTY(VisibleAnywhere)
	int32 MaxBudget = 16000;
	UPROPERTY(EditAnywhere)
	int32 CurrentBudget = 5000;
	 

	class ABotSpawner* BotSpawner = nullptr;
};
