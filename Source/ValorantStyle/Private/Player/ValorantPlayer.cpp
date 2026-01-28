// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ValorantPlayer.h"
#include "Bot/BotSpawner.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Weapon/BaseWeapon.h"
#include "Magazine/Magazine.h"
#include "Weapon/PrimaryGun.h"
#include "Kismet/GameplayStatics.h"
#include "Skill/SkillComponent.h"
#include "Skill/JettSkillComponent.h"
#include "Skill/PhoenixSkillComponent.h"

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
}

// Called when the game starts or when spawned
void AValorantPlayer::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < WeaponClasses.Num(); ++i)
	{
		ABaseWeapon* NewWeapon = GetWorld()->SpawnActor<ABaseWeapon>(WeaponClasses[i]);
		if (i == 0)
		{
			NewWeapon->AttachToComponent(ArmsMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("Gun_Socket"));
		}
		else
		{
			NewWeapon->AttachToComponent(ArmsMesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("Melee_Socket"));
		}
		NewWeapon->SetOwner(this);

		NewWeapon->SetActorHiddenInGame(true);
		NewWeapon->SetActorEnableCollision(false);

		Weapons.Add(NewWeapon);
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

	SetCharaterTypeToJett();
}

// Called every frame
void AValorantPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
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

	CheckUsePassive();
}

// Called to bind functionality to input
void AValorantPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveFoward"), this, & AValorantPlayer::MoveFoward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, & AValorantPlayer::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AValorantPlayer::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &AValorantPlayer::AddControllerYawInput);

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
	PlayerInputComponent->BindAction(TEXT("SkillE"), IE_Pressed, this, &AValorantPlayer::SkillEPressed);
	PlayerInputComponent->BindAction(TEXT("SkillC"), IE_Pressed, this, &AValorantPlayer::SkillCPressed);
	PlayerInputComponent->BindAction(TEXT("SkillUlti"), IE_Pressed, this, &AValorantPlayer::SkillUltiPressed);

	PlayerInputComponent->BindAction(TEXT("PickJett"), IE_Pressed, this, &AValorantPlayer::SetCharaterTypeToJett);
	PlayerInputComponent->BindAction(TEXT("PickPhoenix"), IE_Pressed, this, &AValorantPlayer::SetCharaterTypeToPhoenix);

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
		FPView->SetRelativeLocation(FVector(5.f, 5.f, -165.f));
		FPView->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
		break;
	case 1:
		FPView->SetRelativeLocation(FVector(-25.f, 0.f, -170.f));
		FPView->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
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

void AValorantPlayer::JumpPressed()
{
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
	bInspectWeapon = false;

	// 공격중에는 조사 bool값이 켜지지 않게 해준다. ( 주무기, 보조무기는 Inspect 모션이 없으므로 제외 )
	if (!bSubWeapon && !bSurWeapon && CurrentWeaponIdx == 1)
	{
		bInspectWeapon = true;
	}	
}

void AValorantPlayer::ReloadPressed()
{
	if (CurrentWeapon)
	{
		bTakeReload = CurrentWeapon->TryReload();
	}

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
	bInspectWeapon = false;
	bSurWeapon = true; 
	if (CurrentWeapon)
	{
		CurrentWeapon->SetPullTrigger(true);
	}
}

void AValorantPlayer::ReleaseWeaponSurAbility()
{	
	bSurWeapon = false;

	if (CurrentWeapon)
	{
		CurrentWeapon->SetPullTrigger(false);
	}
}

void AValorantPlayer::UseWeaponSubAbility()
{
	bInspectWeapon = false;
	bSubWeapon = true;
}

void AValorantPlayer::ReleaseWeaponSubAbility()
{
	bSubWeapon = false;
}

void AValorantPlayer::SetCharaterTypeToJett()
{
	UE_LOG(LogTemp, Warning, TEXT("Characher Changed to Jett !!"));
	CurrentCharacterType = ECharacterType::Jett;	
	SetCharacterType(CurrentCharacterType);
}

void AValorantPlayer::SetCharaterTypeToPhoenix()
{
	UE_LOG(LogTemp, Warning, TEXT("Characher Changed to Phoenix !!"));
	CurrentCharacterType = ECharacterType::Phoenix;
	SetCharacterType(CurrentCharacterType);
}

void AValorantPlayer::SkillQPressed()
{
	SkillComponent->UseSkillQ();
}

void AValorantPlayer::SkillEPressed()
{
	SkillComponent->UseSkillE();
}

void AValorantPlayer::SkillCPressed()
{
	SkillComponent->UseSkillC();
}

void AValorantPlayer::SkillUltiPressed()
{
	SkillComponent->UseSkillUlti();
}

void AValorantPlayer::EquipWeapon(int32 Index)
{
	// 무기와 관련된 모든 조건을 꺼준다.
	TurnOffAllCondition();

	if (!Weapons.IsValidIndex(Index)) return;

	if (CurrentWeaponIdx == Index)
		return;

	bSwapWeapon = true;

	if (CurrentWeapon)
	{
		CurrentWeapon->SetActorHiddenInGame(true);
		CurrentWeapon->SetActorEnableCollision(false);
	}

	CurrentWeapon = Weapons[Index];
	CurrentWeapon->SetActorHiddenInGame(false);
	CurrentWeapon->SetActorEnableCollision(true);

	CurrentWeaponIdx = Index;
	AdjustFPView();
}

void AValorantPlayer::TakeReload()
{
	if (!CurrentWeapon)return;
	if (!ArmsMesh) return;

	bTakeReload = false;
	UAnimInstance* Anim = ArmsMesh->GetAnimInstance();
	if (!Anim || !ReloadMontage) return;

	// Reload 애니메이션 몽타주 실행
	if (!Anim->Montage_IsPlaying(ReloadMontage))
	{
		Anim->Montage_Play(ReloadMontage);
	}
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
	if (SkillComponent)
	{
		SkillComponent->DestroyComponent();
	}

	switch (Type)
	{
	case ECharacterType::Jett:
		SkillComponent = NewObject<UJettSkillComponent>(this);
		break;
	case ECharacterType::Phoenix:
		SkillComponent = NewObject<UPhoenixSkillComponent>(this);
		break;
	case ECharacterType::NONE:
		break;
	default:
		break;
	}

	SkillComponent->RegisterComponent();
	SkillComponent->SetOwnerPlayer(this);
}

void AValorantPlayer::CheckUsePassive()
{
	if (SkillComponent)
	{
		if (SkillComponent->IsHavePassiveSkill())
		{
			SkillComponent->UseSkillPassive();
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

bool AValorantPlayer::IsAirborne()
{
	return !GetCharacterMovement()->IsMovingOnGround();
}
