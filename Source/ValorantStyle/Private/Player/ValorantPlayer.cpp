// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ValorantPlayer.h"
#include "Player/ValorantPlayerController.h"
#include "Bot/BotSpawner.h"
#include "Weapon/BaseWeapon.h"
#include "Magazine/Magazine.h"
#include "Weapon/PrimaryGun.h"
#include "Skill/Object/BladeStorm.h"
#include "Skill/SkillComponent.h"
#include "Skill/JettSkillComponent.h"
#include "Skill/PhoenixSkillComponent.h"
#include "UI/ShopItemSlotWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
AValorantPlayer::AValorantPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(RootComponent);
	CameraComp->bUsePawnControlRotation = true;

	FPView = CreateDefaultSubobject<USceneComponent>(TEXT("FPView"));
	FPView->SetupAttachment(CameraComp);

	ArmsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmsMesh"));
	ArmsMesh->SetupAttachment(FPView);
	ArmsMesh->SetOnlyOwnerSee(true);

	ArmsMesh->bCastDynamicShadow = false;
	ArmsMesh->CastShadow = false;

	SkillComponent = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComp"));
	JettSkill = CreateDefaultSubobject<UJettSkillComponent>(TEXT("JettSKill"));
	PhoenixSkill = CreateDefaultSubobject<UPhoenixSkillComponent>(TEXT("PhoenixSkill"));

	FootstepAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("FootstopAudio"));
	FootstepAudio->SetupAttachment(GetRootComponent());
	FootstepAudio->bAutoActivate = false;
}

// Called when the game starts or when spawned
void AValorantPlayer::BeginPlay()
{
	Super::BeginPlay();

	// 근접 무기만 미리 소환
	for (int i = 0; i < WeaponClasses.Num(); ++i)
	{
		
		if (i == 0)
		{
			Weapons.Add(nullptr);
		}
		else
		{
			ABaseWeapon* NewWeapon = GetWorld()->SpawnActor<ABaseWeapon>(WeaponClasses[i]);
			NewWeapon->AttachToComponent(ArmsMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("Melee_Socket"));
			NewWeapon->SetOwner(this);

			NewWeapon->SetWeaponHidden(true);
			NewWeapon->SetActorEnableCollision(false);
			Weapons.Add(NewWeapon);
		}		
	}

	CurrentMagazine = GetWorld()->SpawnActor<AMagazine>(MagazineClass);
	CurrentMagazine->AttachToComponent(ArmsMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("Magazine_Socket"));
	CurrentMagazine->SetActorHiddenInGame(true);

	EquipWeapon(1);

	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	OriginMaxSpeed = GetCharacterMovement()->MaxWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 200.f;

	GetCharacterMovement()->AirControl = 0.8f;
	GetCharacterMovement()->AirControlBoostMultiplier = 2.f;
	GetCharacterMovement()->AirControlBoostVelocityThreshold = 25.f;
	GetCharacterMovement()->BrakingFrictionFactor = 0.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 0.f;

	BotSpawner = Cast<ABotSpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), ABotSpawner::StaticClass()));

	if (FootstepAudio && FootstepSound)
	{
		FootstepAudio->SetSound(FootstepSound);
	}
}

void AValorantPlayer::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), JumplandingSound, GetActorLocation());

}

// Called every frame
void AValorantPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentCharacterType == ECharacterType::NONE)
	{
		SetCharacterType(ECharacterType::Jett);
	}

	// 현재 플레이어의 상태에 따라 Speed값을 갱신한다.
	AdjustSpeed();

	// 반동 카메라 적용
	ApplyRecoil(DeltaTime);

	// 데드존 -> 200
	if (200.f < GetVelocity().Length())
	{
		bMove = true;
	}
	else
	{
		bMove = false;		
	}
	

	if (FootstepAudio)
	{
		const bool bShoudPlay = 10.f < GetVelocity().Size2D() && !bWalk && !bCrouch && !IsAirborne();

		if (bShoudPlay)
		{
			AccumulatedMoveTime += DeltaTime;
			if (AccumulatedMoveTime >= StartThreshold && !FootstepAudio->IsPlaying())
			{
				FootstepAudio->Play();
			}					
		}
		else
		{
			if (FootstepAudio->IsPlaying())
			{
				FootstepAudio->Stop();
			}		
			AccumulatedMoveTime = 0.f;
		}
	}

	if (bHideHands)
	{
		if (!bInvisibleHands)
		{
			HideHands(bChangeWeaponWhenHideOrReveal);
		}

		bHideHands = false;
	}

	if (bRevealHands)
	{
		if (bInvisibleHands)
		{
			RevealHands(bChangeWeaponWhenHideOrReveal);
		}
		bRevealHands = false;
	}

}

// Called to bind functionality to input
void AValorantPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveFoward"), this, &AValorantPlayer::MoveFoward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AValorantPlayer::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AValorantPlayer::MouseLookUp);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &AValorantPlayer::MouseTurn);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AValorantPlayer::JumpPressed);

	PlayerInputComponent->BindAction(TEXT("Walk"), IE_Pressed, this, &AValorantPlayer::WalkPressed);
	PlayerInputComponent->BindAction(TEXT("Walk"), IE_Released, this, &AValorantPlayer::WalkReleased);

	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Pressed, this, &AValorantPlayer::CrouchPressed);
	PlayerInputComponent->BindAction(TEXT("Crouch"), IE_Released, this, &AValorantPlayer::CrouchReleased);

	PlayerInputComponent->BindAction(TEXT("Inspect"), IE_Pressed, this, &AValorantPlayer::InspectPressed);

	PlayerInputComponent->BindAction(TEXT("UseWeaponSur"), IE_Pressed, this, &AValorantPlayer::UseWeaponSurAbility);
	PlayerInputComponent->BindAction(TEXT("UseWeaponSur"), IE_Released, this, &AValorantPlayer::ReleaseWeaponSurAbility);

	PlayerInputComponent->BindAction(TEXT("UseWeaponSub"), IE_Pressed, this, &AValorantPlayer::UseWeaponSubAbility);
	PlayerInputComponent->BindAction(TEXT("UseWeaponSub"), IE_Released, this, &AValorantPlayer::ReleaseWeaponSubAbility);

	PlayerInputComponent->BindAction(TEXT("SwapPrimary"), IE_Pressed, this, &AValorantPlayer::SelectWeapon0);
	PlayerInputComponent->BindAction(TEXT("SwapMelee"), IE_Pressed, this, &AValorantPlayer::SelectWeapon1);

	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &AValorantPlayer::ReloadPressed);
	PlayerInputComponent->BindAction(TEXT("DummyBotMaxShiledTrigger"), IE_Pressed, this, &AValorantPlayer::DummyMaxShieldTriggerPressed);
	PlayerInputComponent->BindAction(TEXT("DummyBotNormalShiledTrigger"), IE_Pressed, this, &AValorantPlayer::DummyNormalShieldTriggerPressed);

	PlayerInputComponent->BindAction(TEXT("SkillQ"), IE_Pressed, this, &AValorantPlayer::SkillQPressed);
	PlayerInputComponent->BindAction(TEXT("SkillQ"), IE_Released, this, &AValorantPlayer::SkillQReleased);
	PlayerInputComponent->BindAction(TEXT("SkillE"), IE_Pressed, this, &AValorantPlayer::SkillEPressed);
	PlayerInputComponent->BindAction(TEXT("SkillE"), IE_Released, this, &AValorantPlayer::SkillEReleased);
	PlayerInputComponent->BindAction(TEXT("SkillC"), IE_Pressed, this, &AValorantPlayer::SkillCPressed);
	PlayerInputComponent->BindAction(TEXT("SkillC"), IE_Released, this, &AValorantPlayer::SkillCReleased);
	PlayerInputComponent->BindAction(TEXT("SkillUlti"), IE_Pressed, this, &AValorantPlayer::SkillUltiPressed);
	PlayerInputComponent->BindAction(TEXT("SkillUlti"), IE_Released, this, &AValorantPlayer::SkillUltiReleased);

	PlayerInputComponent->BindAction(TEXT("SubShield"), IE_Pressed, this, &AValorantPlayer::SubShield);

	//PlayerInputComponent->BindAction(TEXT("DropWeapon"), IE_Pressed, this, &AValorantPlayer::DropWeapon);
}

void AValorantPlayer::PlayerGetKill()
{
	// 플레이어가 킬 한 정보를 스킬에 넘긴다
	SkillComponent->GetKill();

	// 크레딧 추가
	CurrentBudget += 200;
	if (CurrentBudget > MaxBudget)
	{
		CurrentBudget = MaxBudget;
	}
}

void AValorantPlayer::SetHandMeshRelativeLocationRotaiton(const FVector& Location, const FRotator& Rotation)
{
	FPView->SetRelativeLocation(AdjustLocation + Location);
	FPView->SetRelativeRotation(AdjustRotation + Rotation);
}

void AValorantPlayer::BuyItem(const FString& ItemName, int32 Price)
{
	CurrentBudget -= Price;

	// 이전 자리에 있던 아이템 기억 -> Resell에서 되돌려준다.

	if (ItemName == "VANDAL")
	{
		UE_LOG(LogTemp, Warning, TEXT("Buy Vandal"));

		// 이전 무기 정보 저장
		PrevPlayerEquipment.EquippedPrimary = CurrentPlayerEquipment.EquippedPrimary;
		PrevPlayerEquipment.Primary = CurrentPlayerEquipment.Primary;
		PrevPlayerEquipment.PrimaryPrice = CurrentPlayerEquipment.PrimaryPrice;

		// 현재 아이템을 사용하지 않은 상태면 판매
		if (IsItemCanSelled(CurrentPlayerEquipment.EquippedPrimary, EShopItemType::Weapon))
		{
			ResellItem(CurrentPlayerEquipment.EquippedPrimary, CurrentPlayerEquipment.PrimaryPrice);
		}
		// 현재 아이템이 장착중이지만 사용했다면 (새로운 무기로)
		else if (IsItemEquipped(CurrentPlayerEquipment.EquippedPrimary, EShopItemType::Weapon))
		{
			EquipWeapon(1);
			Weapons[0] = nullptr;
		}
	
		// 무기 소환
		ABaseWeapon* NewWeapon = GetWorld()->SpawnActor<ABaseWeapon>(WeaponClasses[0]);
		NewWeapon->AttachToComponent(ArmsMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("Gun_Socket"));
		NewWeapon->SetOwner(this);
		NewWeapon->SetWeaponHidden(true);
		NewWeapon->SetActorEnableCollision(false);
		Weapons[0] = NewWeapon;

		// 구입한 무기 정보 저장
		CurrentPlayerEquipment.EquippedPrimary = TEXT("VANDAL");
		CurrentPlayerEquipment.Primary = NewWeapon;
		CurrentPlayerEquipment.PrimaryPrice = Price;

		// 현재 무기 바로 장착
		EquipWeapon(0);
	}
	
	if (ItemName == "Light Shield")
	{
		UE_LOG(LogTemp, Warning, TEXT("Buy Light Shield"));

		// 현재 아이템들을 이전의 아이템으로 기억한다
		PrevPlayerEquipment.EquippedShield = CurrentPlayerEquipment.EquippedShield;

		PrevPlayerEquipment.MaxShield = CurrentPlayerEquipment.MaxShield;
		PrevPlayerEquipment.CurrentShield = CurrentPlayerEquipment.CurrentShield;
		PrevPlayerEquipment.ShieldPrice = CurrentPlayerEquipment.ShieldPrice;

		// 현재 아이템이 있고 사용하지 않은 상태라면 Sell
		if (IsItemCanSelled(CurrentPlayerEquipment.EquippedShield, EShopItemType::Shield))
		{
			ResellItem(CurrentPlayerEquipment.EquippedShield, CurrentPlayerEquipment.ShieldPrice);
		}
		
		// 현재 구매한 아이템으로 적용
		CurrentPlayerEquipment.EquippedShield = TEXT("Light Shield");
		
		CurrentPlayerEquipment.MaxShield = 25;
		CurrentPlayerEquipment.CurrentShield = 25;
		CurrentPlayerEquipment.ShieldPrice = Price;
	}
	if (ItemName == "Heavy Shield")
	{
		UE_LOG(LogTemp, Warning, TEXT("Buy Heavy Shield"));


		PrevPlayerEquipment.EquippedShield = CurrentPlayerEquipment.EquippedShield;

		PrevPlayerEquipment.MaxShield = CurrentPlayerEquipment.MaxShield;
		PrevPlayerEquipment.CurrentShield = CurrentPlayerEquipment.CurrentShield;
		PrevPlayerEquipment.ShieldPrice = CurrentPlayerEquipment.ShieldPrice;

		// 현재 아이템이 있고 사용하지 않은 상태라면 Sell
		if (IsItemCanSelled(CurrentPlayerEquipment.EquippedShield, EShopItemType::Shield))
		{
			ResellItem(CurrentPlayerEquipment.EquippedShield, CurrentPlayerEquipment.ShieldPrice);
		}

		CurrentPlayerEquipment.EquippedShield = TEXT("Heavy Shield");

		CurrentPlayerEquipment.MaxShield = 50;
		CurrentPlayerEquipment.CurrentShield = 50;
		CurrentPlayerEquipment.ShieldPrice = Price;
	}

	if (ItemName == "Skill Q")
	{
		UE_LOG(LogTemp, Warning, TEXT("Buy SKILL Q"));
		SkillComponent->BuySkillQ();
	}
	if (ItemName == "Skill C")
	{
		UE_LOG(LogTemp, Warning, TEXT("Buy SKILL C"));
		SkillComponent->BuySkillC();
	}
	if (ItemName == "Skill E")
	{
		UE_LOG(LogTemp, Warning, TEXT("Buy SKILL E"));
		SkillComponent->BuySkillE();
	}

	OnEquipmentChanged.Broadcast();
}

void AValorantPlayer::ResellItem(const FString& ItemName, int32 Price)
{
	CurrentBudget += Price;

	if (ItemName == "Vandal")
	{
		UE_LOG(LogTemp, Warning, TEXT("Resell Vandal"));

		CurrentPlayerEquipment.EquippedPrimary = TEXT("NONE");
		CurrentPlayerEquipment.Primary = nullptr;
		CurrentPlayerEquipment.PrimaryPrice = 0;
	
		// 다음무기로
		EquipWeapon(1);
		
		// 무기 삭제
		Weapons[0]->Destroy();
		Weapons[0] = nullptr;
	}

	if (ItemName == "Light Shield" || ItemName == "Heavy Shield")
	{
		UE_LOG(LogTemp, Warning, TEXT("Resell Shield"));
		
		CurrentPlayerEquipment.EquippedShield = TEXT("NONE");
		CurrentPlayerEquipment.MaxShield = 0.f;
		CurrentPlayerEquipment.CurrentShield = 0.f;
		CurrentPlayerEquipment.ShieldPrice = 0;
	}

	if (ItemName == "Skill Q")
	{
		UE_LOG(LogTemp, Warning, TEXT("Resell SKILL Q"));
		SkillComponent->ResellSkillQ();
	}
	if (ItemName == "Skill C")
	{
		UE_LOG(LogTemp, Warning, TEXT("Resell SKILL C"));
		SkillComponent->ResellSkillC();
	}
	if (ItemName == "Skill E")
	{
		UE_LOG(LogTemp, Warning, TEXT("Resell SKILL E"));
		SkillComponent->ResellSkillE();
	}

	OnEquipmentChanged.Broadcast();
}

bool AValorantPlayer::IsItemCanSelled(const FString& ItemName, EShopItemType ItemType)
{
	if (ItemName == "NONE")
	{
		return false;
	}

	switch (ItemType)
	{
	case EShopItemType::Weapon:
		if (ItemName == CurrentPlayerEquipment.EquippedPrimary)
		{
			// 해당 아이템을 갖고있고 한번도 사용한적이 없다면
			ABaseWeapon* PrimaryWeapon = Weapons[0];
			// 총알이 사용됐으면 사용한 총기
			if (PrimaryWeapon->GetCurrentAmmo() < PrimaryWeapon->GetMaxAmmo()
				|| PrimaryWeapon->GetCurrentLeftAmmo() < PrimaryWeapon->GetMaxLeftAmmo())
			{
				return false;
			}
			return true;
		}
		else
		{
			return false;
		}
	case EShopItemType::Shield:
		if (ItemName == CurrentPlayerEquipment.EquippedShield)
		{
			// 해당 아이템을 갖고있고 한번도 사용한적이 없다면
			if (CurrentPlayerEquipment.CurrentShield < CurrentPlayerEquipment.MaxShield)
			{
				// 현재 쉴드가 최대 쉴드 보다 적다면 쉴드가 사용됨
				return false;
			}
			return true;
		}
		else
		{
			return false;
		}
	case EShopItemType::Skill:
		if (ItemName == "SKILL Q")
		{
			// 현재 개수가 최소개수
			if (0 < SkillComponent->GetSkillQCount())
			{
				UE_LOG(LogTemp, Display, TEXT("current Skill Q count : %d"), SkillComponent->GetSkillQCount());
				return true;
			}
			else
			{
				UE_LOG(LogTemp, Display, TEXT("Cant Sell"));
				return false;
			}
		}
		if (ItemName == "SKILL E")
		{
			// 현재 개수가 최대 개수와 같으면 true
			if (1 < SkillComponent->GetSkillECount())
			{
				UE_LOG(LogTemp, Display, TEXT("current Skill E count : %d"), SkillComponent->GetSkillECount());
				return true;
			}
			else
			{
				UE_LOG(LogTemp, Display, TEXT("Cant Sell"));
				return false;
			}
		}
		if (ItemName == "SKILL C")
		{
			// 현재 개수가 최대 개수와 같으면 true
			if (0 < SkillComponent->GetSkillCCount())
			{
				UE_LOG(LogTemp, Display, TEXT("current Skill C count : %d"), SkillComponent->GetSkillCCount());
				return true;
			}
			else
			{
				UE_LOG(LogTemp, Display, TEXT("Cant Sell"));
				return false;
			}
		}
	case EShopItemType::NONE:
		return false;
	}

	return false;
}

void AValorantPlayer::SelectCharacter()
{
	OnCharacterSelectChanged.Broadcast();
}

bool AValorantPlayer::IsItemEquipped(const FString& ItemName, EShopItemType ItemType)
{	
	switch (ItemType)
	{
	case EShopItemType::Weapon:
		if (ItemName == CurrentPlayerEquipment.EquippedPrimary)
		{
			return true;
		}
		else
		{
			return false;
		}
	case EShopItemType::Shield:
		if (ItemName == CurrentPlayerEquipment.EquippedShield)
		{
			return true;						
		}
		else
		{
			return false;
		}
	case EShopItemType::Skill:
		if (ItemName == "SKILL Q")
		{
			
			// 현재 개수가 최대 개수와 같으면 true
			if (SkillComponent->GetSkillMaxQCount() <= SkillComponent->GetSkillQCount())
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		if (ItemName == "SKILL E")
		{
			// 현재 개수가 최대 개수와 같으면 true
			if (SkillComponent->GetSkillMaxECount() <= SkillComponent->GetSkillECount())
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		if (ItemName == "SKILL C")
		{
			// 현재 개수가 최대 개수와 같으면 true
			if (SkillComponent->GetSkillMaxCCount() <= SkillComponent->GetSkillCCount())
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	case EShopItemType::NONE:
		return false;
	}

	return false;
}

void AValorantPlayer::AdjustSpeed()
{
	if (bWalk)
	{
		Speed = 0.5f;
	}
	else
	{
		Speed = 1.f;
	}
	GetCharacterMovement()->MaxWalkSpeed = OriginMaxSpeed * Speed;
}

void AValorantPlayer::AdjustFPView()
{
	switch (CurrentWeaponIdx)
	{
	case 0:
		AdjustLocation = FVector(5.f, 5.f, -165.f);
		AdjustRotation = FRotator(0.f, 90.f, 0.f);
		FPView->SetRelativeLocation(FVector(5.f, 5.f, -165.f));
		FPView->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
		break;
	case 1:
		AdjustLocation = FVector(-25.f, 0.f, -170.f);
		AdjustRotation = FRotator(0.f, 90.f, 0.f);
		FPView->SetRelativeLocation(AdjustLocation);
		FPView->SetRelativeRotation(AdjustRotation);
		break;
	}
}

void AValorantPlayer::MoveFoward(float AxisValue)
{
	AddMovementInput(GetActorForwardVector() * AxisValue);
}

void AValorantPlayer::MoveRight(float AxisValue)
{
	AddMovementInput(GetActorRightVector() * AxisValue);
}

void AValorantPlayer::MouseLookUp(float Value)
{
	if (AValorantPlayerController* PC = Cast<AValorantPlayerController>(GetController()))
	{
		AddControllerPitchInput(Value * PC->MouseSensitivity);
	}	
}

void AValorantPlayer::MouseTurn(float Value)
{
	if (AValorantPlayerController* PC = Cast<AValorantPlayerController>(GetController()))
	{
		AddControllerYawInput(Value * PC->MouseSensitivity);
	}
}

void AValorantPlayer::JumpPressed()
{
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), JumpStartSound, GetActorLocation());

	if (bIsCrouched)
	{
		UnCrouch();

		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ACharacter::Jump);
		return;
	}

	Jump();
}

void AValorantPlayer::WalkPressed()
{
	bWalk = true;
}

void AValorantPlayer::WalkReleased()
{
	bWalk = false;
}

void AValorantPlayer::CrouchPressed()
{
	bCrouch = true;
	Crouch();
}

void AValorantPlayer::CrouchReleased()
{
	bCrouch = false;
	UnCrouch();
}

void AValorantPlayer::InspectPressed()
{
	if (!CurrentWeapon || CurrentWeapon->IsHidden())
	{
		return;
	}

	bInspectWeapon = false;

	// 공격중에는 조사 bool값이 켜지지 않게 해준다. ( 주무기, 보조무기는 Inspect 모션이 없으므로 제외 )
	if (!bSubWeapon && !bSurWeapon && CurrentWeaponIdx == 1)
	{
		bInspectWeapon = true;
	}
}

void AValorantPlayer::ReloadPressed()
{
	if (!CurrentWeapon || CurrentWeapon->IsHidden())
	{
		return;
	}

	bTakeReload = CurrentWeapon->TryReload();

	if (bTakeReload)
	{
		TakeReload();
	}
}

void AValorantPlayer::DummyMaxShieldTriggerPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Dummy Has Max Shiled !!"));
	BotSpawner->ToggleDummyShiled(true);
}

void AValorantPlayer::DummyNormalShieldTriggerPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Dummy Has Normal Shiled !!"));
	BotSpawner->ToggleDummyShiled(false);
}

void AValorantPlayer::UseWeaponSurAbility()
{
	if (SkillComponent)
	{
		SkillComponent->PressMouseLBTN();
	}
	if (!CurrentWeapon || CurrentWeapon->IsHidden())
	{
		return;
	}
	bInspectWeapon = false;
	bSurWeapon = true;
	CurrentWeapon->SetPullTrigger(true);
}

void AValorantPlayer::ReleaseWeaponSurAbility()
{
	if (SkillComponent)
	{
		SkillComponent->ReleaseMouseLBTN();
	}

	bSurWeapon = false;
	if (CurrentWeapon)
	{
		CurrentWeapon->SetPullTrigger(false);
	}
}

void AValorantPlayer::UseWeaponSubAbility()
{
	if (SkillComponent)
	{
		SkillComponent->PressMouseRBTN();
	}

	if (!CurrentWeapon || CurrentWeapon->IsHidden())
	{
		return;
	}
	bInspectWeapon = false;
	bSubWeapon = true;
}

void AValorantPlayer::ReleaseWeaponSubAbility()
{
	if (SkillComponent)
	{
		SkillComponent->ReleaseMouseRBTN();
	}
	bSubWeapon = false;
}

//void AValorantPlayer::DropWeapon()
//{
//	// 무기가 없거나 근접무기면 return
//	if (!CurrentWeapon || CurrentWeaponIdx == 2)
//	{
//		return;
//	}
//	
//	ABaseWeapon* WeaponToDrop = CurrentWeapon;
//	int32 DropIdx = CurrentWeaponIdx;		
//
//	// Detach
//	WeaponToDrop->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
//	WeaponToDrop->SetDrop(true);
//
//	FVector DropLocation = GetActorLocation() + GetActorForwardVector() * 100.f + FVector(0, 0, -30.f);
//	WeaponToDrop->SetActorLocation(DropLocation);
//	WeaponToDrop->SetActorRotation(GetActorRotation());
//	WeaponToDrop->SetActorEnableCollision(true);
//
//	if (WeaponToDrop->GetWeaponMesh())
//	{
//		UPrimitiveComponent* WeaponMesh = WeaponToDrop->GetWeaponMesh();
//
//		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
//		WeaponMesh->SetCollisionObjectType(ECC_PhysicsBody);
//
//		WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
//
//		WeaponMesh->SetGenerateOverlapEvents(true);
//		WeaponMesh->SetSimulatePhysics(true);
//		WeaponMesh->SetEnableGravity(true);
//
//		WeaponMesh->SetLinearDamping(2.f);
//		WeaponMesh->SetAngularDamping(3.f);
//
//		FVector Forward = GetActorForwardVector();
//		FVector Up = FVector::UpVector;
//
//		FVector Impulse = (Forward * 300.f) + (Up * 300.f);
//
//		WeaponMesh->AddImpulse(Impulse, NAME_None, true);
//	}
//
//	// 해당 무기 자리 nullptr로
//	CurrentWeapon = nullptr;
//	Weapons[DropIdx] = nullptr;
//
//	EquipWeapon(1);
//}

void AValorantPlayer::HideHands(bool bChgangeWeapon)
{
	bInvisibleHands = true;
	if (!ArmsMesh || !CurrentWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("Cant Find Arms or Wepaon"));
		return;
	}

	StopReload();
	// 현재의 armmesh를 숨긴다.
	ArmsMesh->SetHiddenInGame(true);
	CurrentWeapon->SetActorHiddenInGame(true);

	// 무기를 없앤다.	
	if (bChgangeWeapon)
	{
		TemporaryWeaponIdx = CurrentWeaponIdx;
		EquipWeapon(-1);
	}

	bCanSwapWeapon = false;
	bChangeWeaponWhenHideOrReveal = false;
}

void AValorantPlayer::RevealHands(bool bChangeWeapon)
{
	bInvisibleHands = false;
	bCanSwapWeapon = true;
	if (!ArmsMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("Cant Find Arms"));
		return;
	}

	// armesh를 다시 보이게한다
	ArmsMesh->SetHiddenInGame(false);

	// 이전의 무기를 다시 보이게한다
	if (bChangeWeapon)
	{
		EquipWeapon(TemporaryWeaponIdx);
		TemporaryWeaponIdx = -1;
	}

	if (!CurrentWeapon)
	{
		return;
	}
	
	CurrentWeapon->SetActorHiddenInGame(false);


	bChangeWeaponWhenHideOrReveal = false;
}

void AValorantPlayer::SkillQPressed()
{
	SkillComponent->UseSkillQ();
}

void AValorantPlayer::SkillQReleased()
{
	SkillComponent->ReleaseSKillQ();
}

void AValorantPlayer::SkillEPressed()
{
	SkillComponent->UseSkillE();
}

void AValorantPlayer::SkillEReleased()
{
	SkillComponent->ReleaseSKillE();
}

void AValorantPlayer::SkillCPressed()
{
	SkillComponent->UseSkillC();
}

void AValorantPlayer::SkillCReleased()
{
	SkillComponent->ReleaseSKillC();
}

void AValorantPlayer::SkillUltiPressed()
{
	SkillComponent->UseSkillUlti();
}

void AValorantPlayer::SkillUltiReleased()
{
	SkillComponent->ReleaseSKillUlti();
}

void AValorantPlayer::EquipWeapon(int32 Index)
{
	// 무기와 관련된 모든 조건을 꺼준다.
	TurnOffAllCondition();
	StopReload();

	if (CurrentWeaponIdx == Index && !bInvisibleHands)
	{
		UE_LOG(LogTemp, Warning, TEXT("Same Weapon"));
		return;
	}

	if (!bCanSwapWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant Swap"));
		return;
	}


	// 현재 무기를 없앤다
	if (Index == -1)
	{
		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwnerPlayer(nullptr);
			CurrentWeapon->SetWeaponHidden(true);
			CurrentWeapon->SetActorEnableCollision(false);
			UE_LOG(LogTemp, Display, TEXT("Hide Weapon"));
		}

		CurrentWeapon = nullptr;
	}
	// 다른 무기로 교체
	else
	{
		// 해당 무기칸이 비어있다면 교체 X
		if (!Weapons.IsValidIndex(Index) || !Weapons[Index])
		{
			UE_LOG(LogTemp, Error, TEXT("Cant Equip Weapon : %d"), Index);
			return;
		}
	
		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwnerPlayer(nullptr);
			CurrentWeapon->SetWeaponHidden(true);
			CurrentWeapon->SetActorEnableCollision(false);
			UE_LOG(LogTemp, Display, TEXT("Hide Weapon"));
		}

		bSwapWeapon = true;
		CurrentWeapon = Weapons[Index];
		CurrentWeapon->SetOwnerPlayer(this);
		CurrentWeapon->SetWeaponHidden(false);
		CurrentWeapon->SetActorEnableCollision(true);
		UE_LOG(LogTemp, Display, TEXT("Equip Weapon : %d"), Index);
	}

	CurrentWeaponIdx = Index;
	AdjustFPView();
}

void AValorantPlayer::TakeReload()
{
	if (!CurrentWeapon)
	{
		return;
	}
	if (CurrentWeapon->IsHidden())
	{
		return;
	}
	if (!ArmsMesh)
	{
		return;
	}

	bTakeReload = false;
	UAnimInstance* Anim = ArmsMesh->GetAnimInstance();
	if (!Anim || !ReloadMontage)
	{
		return;
	}

	// Reload 애니메이션 몽타주 실행
	if (!Anim->Montage_IsPlaying(ReloadMontage))
	{
		Anim->Montage_Play(ReloadMontage);
	}
}

void AValorantPlayer::StopReload()
{
	UAnimInstance* Anim = ArmsMesh->GetAnimInstance();
	if (!Anim || !ReloadMontage)
	{
		return;
	}

	if (!CurrentMagazine)
	{
		return;
	}
	if (!CurrentWeapon)
	{
		return;
	}

	// 새로운 탄창 비 활성화
	CurrentMagazine->SetActorHiddenInGame(true);
	CurrentWeapon->StopReload();

	// Reload 애니메이션 몽타주 정지
	if (Anim->Montage_IsPlaying(ReloadMontage))
	{
		Anim->Montage_Stop(0.f, ReloadMontage);
	}
}

void AValorantPlayer::SelectWeapon0()
{
	if (SkillComponent)
	{
		SkillComponent->CheckPlayerKeyInput(EKeys::One);
	}
	EquipWeapon(0);
}

void AValorantPlayer::SelectWeapon1()
{
	if (SkillComponent)
	{
		SkillComponent->CheckPlayerKeyInput(EKeys::Two);
	}
	EquipWeapon(1);
}

void AValorantPlayer::ApplyRecoil(float DeltaTime)
{
	if (!CurrentWeapon)
	{
		return;
	}

	// 현재 카메라가 향해야할 반동값
	FVector2D Target = CurrentWeapon->CurrentRecoilOffset;

	// 카메라 이동
	FVector2D NewCameraRecoil = FMath::Vector2DInterpTo(CurrentCameraRecoil, Target, DeltaTime, CurrentWeapon->RecoilRiseSpeed);
	FVector2D DeltaRecoil = NewCameraRecoil - LastAppliedRecoil;

	// 적용
	AddControllerPitchInput(-DeltaRecoil.Y);
	AddControllerYawInput(DeltaRecoil.X);

	CurrentCameraRecoil = NewCameraRecoil;
	LastAppliedRecoil = NewCameraRecoil;

	// 사격중이 아니라면 원래대로
	if (GetCurWeaponState() != EWeaponState::Firing)
	{
		CurrentWeapon->CurrentRecoilOffset = FMath::Vector2DInterpTo(CurrentWeapon->CurrentRecoilOffset, FVector2D::ZeroVector, DeltaTime, CurrentWeapon->RecoilRecoverSpeed);
	}
}

void AValorantPlayer::SetCharacterType(ECharacterType Type)
{
	CurrentCharacterType = Type;

	switch (Type)
	{
	case ECharacterType::Jett:
		SkillComponent = JettSkill;
		break;
	case ECharacterType::Phoenix:
		SkillComponent = PhoenixSkill;
		break;
	case ECharacterType::NONE:
		SkillComponent = nullptr;
		break;
	default:
		break;
	}

	if (SkillComponent)
	{
		SkillComponent->SetOwnerPlayer(this);
		SkillComponent->CharacterSelected();
		
		OnSkillComponentChanged.Broadcast(SkillComponent);
	}
}


void AValorantPlayer::SubShield()
{
	if (CurrentPlayerEquipment.EquippedShield != "NONE")
	{
		CurrentPlayerEquipment.CurrentShield -= 10;
		if (CurrentPlayerEquipment.CurrentShield <= 0)
		{
			CurrentPlayerEquipment.CurrentShield = 0;
		}
	}
}


void AValorantPlayer::StartReload()
{
	if (!CurrentMagazine) return;
	// 새로운 탄창 활성화
	CurrentMagazine->SetActorHiddenInGame(false);

	APrimaryGun* Primary = Cast<APrimaryGun>(CurrentWeapon);
	if (!Primary) return;
	Primary->StartReload();
}

void AValorantPlayer::EndReload()
{
	if (!CurrentMagazine) return;

	// 탄창 비활성화
	CurrentMagazine->SetActorHiddenInGame(true);

	// 탄창을 총에 결합한 순간
	CurrentWeapon->EndReload();
}

void AValorantPlayer::AllAnimEndReload()
{
	// 모든 애니메이션 종료 -> 사격 가능
	CurrentWeapon->AllAnimEndReload();
}

void AValorantPlayer::EndDraw()
{
	bSwapWeapon = false;
}

void AValorantPlayer::TurnOffAllCondition()
{
	bSurWeapon = false;
	bSubWeapon = false;
	bInspectWeapon = false;
	bSwapWeapon = false;
}

bool AValorantPlayer::IsInspectWeapon() const
{
	return bInspectWeapon;
}

void AValorantPlayer::SetInspectWeapon(bool bInspect)
{
	bInspectWeapon = bInspect;
}

bool AValorantPlayer::IsUseSurWeaponAbility() const
{
	return bSurWeapon;
}

bool AValorantPlayer::IsUseSubWeaponAbility() const
{
	return bSubWeapon;
}

bool AValorantPlayer::IsSwapWeapon() const
{
	return bSwapWeapon;
}

void AValorantPlayer::SetSwapWeapon(bool bSwap)
{
	bSwapWeapon = bSwap;
}

int32 AValorantPlayer::GetCurWeaponIdx() const
{
	return CurrentWeaponIdx;
}

EWeaponState AValorantPlayer::GetCurWeaponState() const
{
	if (!CurrentWeapon)
	{
		return EWeaponState::NONE;
	}

	return CurrentWeapon->GetCurWeaponState();
}

float AValorantPlayer::GetCurrentHealth() const
{
	return Health;
}

float AValorantPlayer::GetCurrentShiled() const
{
	return CurrentPlayerEquipment.CurrentShield;
}

float AValorantPlayer::GetCurrentShiledPercent() const
{
	if (CurrentPlayerEquipment.MaxShield == 0)
		return 0.f;

	return CurrentPlayerEquipment.CurrentShield / CurrentPlayerEquipment.MaxShield;
}

ABaseWeapon* AValorantPlayer::GetCurrentWeapon() const
{
	return CurrentWeapon;
}

void AValorantPlayer::RequestHideHands(bool ChangeWeapon)
{
	bHideHands = true;
	bChangeWeaponWhenHideOrReveal = ChangeWeapon;
}

void AValorantPlayer::RequestRevealHands(bool ChangeWeapon)
{
	bRevealHands = true;
	bChangeWeaponWhenHideOrReveal = ChangeWeapon;
}

bool AValorantPlayer::IsAirborne()
{
	return !GetCharacterMovement()->IsMovingOnGround();
}

