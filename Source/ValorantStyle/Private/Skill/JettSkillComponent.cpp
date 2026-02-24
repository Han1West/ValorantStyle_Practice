// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/JettSkillComponent.h"
#include "Player/ValorantPlayer.h"
#include "Skill/Object/Cloudburst.h"
#include "Skill/Object/BladeStorm.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"


UJettSkillComponent::UJettSkillComponent()
{
	static ConstructorHelpers::FClassFinder<ACloudburst> CloudburstFinder(TEXT("/Game/BP_Cloudburst"));
	static ConstructorHelpers::FClassFinder<ABladeStorm> BladestormFinder(TEXT("/Game/BP_Bladestorm"));

	static ConstructorHelpers::FObjectFinder<UTexture2D> QTextureObj(TEXT("/Game/UIAsset/Jett_Q"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> ETextureObj(TEXT("/Game/UIAsset/Jett_E"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> CTextureObj(TEXT("/Game/UIAsset/Jett_C"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> UltiTextureObj(TEXT("/Game/UIAsset/Jett_Ulti"));
	
	static ConstructorHelpers::FObjectFinder<USoundBase> FlyStartSoundFinder(TEXT("/Game/SoundAsset/effect/Skill/Jett_fly_start"));
	static ConstructorHelpers::FObjectFinder<USoundBase> FlyPersistSoundFinder(TEXT("/Game/SoundAsset/effect/Skill/Jett_fly_persist"));
	static ConstructorHelpers::FObjectFinder<USoundBase> DashSoundFinder(TEXT("/Game/SoundAsset/effect/Skill/jett_dash"));
	static ConstructorHelpers::FObjectFinder<USoundBase> UpDraftSoundFinder(TEXT("/Game/SoundAsset/effect/Skill/jett_updraft"));
	static ConstructorHelpers::FObjectFinder<USoundBase> UltiSoundFinder(TEXT("/Game/SoundAsset/effect/Skill/jett_ulti"));	
	static ConstructorHelpers::FObjectFinder<USoundBase> UltiShotSoundFinder(TEXT("/Game/SoundAsset/effect/Skill/bladestorm_shot"));
	static ConstructorHelpers::FObjectFinder<USoundBase> UltiAllShotSoundFinder(TEXT("/Game/SoundAsset/effect/Skill/bladestorm_allshot"));

	// 스킬 오브젝트 load
	if (CloudburstFinder.Succeeded())
	{
		CloudburstClass = CloudburstFinder.Class;
	}
	if (BladestormFinder.Succeeded())
	{
		BladestormClass = BladestormFinder.Class;
	}	

	// 스킬 이미지 load
	if (QTextureObj.Succeeded())
	{
		QSkillIconTexture = QTextureObj.Object;
	}
	if (ETextureObj.Succeeded())
	{
		ESkillIconTexture = ETextureObj.Object;
	}
	if (CTextureObj.Succeeded())
	{
		CSkillIconTexture = CTextureObj.Object;
	}
	if (UltiTextureObj.Succeeded())
	{
		UltiSkillIconTexture = UltiTextureObj.Object;
	}

	// 스킬 사운드 load
	if (FlyStartSoundFinder.Succeeded())
	{
		FlyStartSound = FlyStartSoundFinder.Object;
	}
	if (FlyPersistSoundFinder.Succeeded())
	{
		FlyPersistSound = FlyPersistSoundFinder.Object;
	}
	if (DashSoundFinder.Succeeded())
	{
		DashSound = DashSoundFinder.Object;
	}
	if (UpDraftSoundFinder.Succeeded())
	{
		UpdraftSound = UpDraftSoundFinder.Object;
	}
	if (UltiSoundFinder.Succeeded())
	{
		DrawBladestormSound = UltiSoundFinder.Object;
	}
	if (UltiShotSoundFinder.Succeeded())
	{
		ShotBladestormSound = UltiShotSoundFinder.Object;
	}
	if (UltiAllShotSoundFinder.Succeeded())
	{
		ShotAllBladestormSound = UltiAllShotSoundFinder.Object;
	}


	QSkillText = TEXT("UPDRAFT");
	ESkillText = TEXT("TAILWIND");
	CSkillText = TEXT("CLOUD BURST");
	UltiSkillText = TEXT("BLADE STROM");

	QSkillPrice = 200;
	ESkillPrice = 250;
	CSkillPrice = 100;

	CharacterClass = ECharacterClass::Duelist;
}

void UJettSkillComponent::BeginPlay()
{
	bPassive = true;

	SkillMaxQCount = 1;
	SkillQCount = 1;

	SkillMaxECount = 1;
	SkillECount = 1;

	SkillMaxCCount = 2;
	SkillCCount = 2;

	NeedUltimateCount = 7;
	CurrentUltimateCount = 0;

	SkillQCastingTime = 1.f;
	SkillECastingTime = 1.f;
	SkillCCastingTime = 0.4f;

	SkillEDurationTime = 10.f;
}

void UJettSkillComponent::CharacterSelected()
{
	Super::CharacterSelected();

	// 제트 칼날 폭풍 미리 소환
	for (int i = 0; i < 5; ++i)
	{
		ABladeStorm* NewBladestorm = GetWorld()->SpawnActor<ABladeStorm>(BladestormClass);
		if (NewBladestorm == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Cant Load Bladestorm"));
			return;
		}

		NewBladestorm->AttachToComponent(OwnerPlayer->GetCameraComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		FTransform NewTransform = GetBladeTransformForView(i);
		NewTransform.SetScale3D(NewBladestorm->GetActorScale3D());
		NewBladestorm->SetActorTransform(NewTransform);
		NewBladestorm->SetActorRotation(FRotator(-20.f, 180.f, 0.f));
		NewBladestorm->SetInitialRelativeLocation();
		NewBladestorm->SetAttachedOwnerPlayer(OwnerPlayer);
		NewBladestorm->SetActorHiddenInGame(true);
		NewBladestorm->SetActorEnableCollision(false);
		NewBladestorm->SetAttached(true);

		Bladestorms.Add(NewBladestorm);
	}
}


void UJettSkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bTailWind)
	{
		CurrentActiveAccTime += DeltaTime;		
		// 일정 시간안에 스킬을 다시 발동시키지 않으면 초기화
		if (CurrentActiveAccTime > CurrentActiveDurationTime)
		{
			CurrentActiveAccTime = 0.f;
			CurrentActiveDurationTime = 0.f;
			bActiveHasDurationSkill = false;
			bTailWind = false;
			SkillECount--;
		}		
	}

	// C키를 지속적으로 누른상태라면 (한번도 떼지 않고)
	if (bHoldingC && ControlledCloudburst)
	{
		ControlledTime += DeltaTime;
		UpdateCloudburstDirection();
	}

	if (bSkillQInput)
	{
		// 해당 스킬의 캐스팅 시간 소모
		OwnerPlayer->GetWorldTimerManager().SetTimer(CastingQHandle, this, &USkillComponent::CastingEndQSkil, SkillQCastingTime, false);
		bSkillQInput = false;

		if (bSkillUltiCasting)
		{
			DeSpawnBladeStorm();
			bWasUsingUlti = true;
		}
	}
	if (bSkillEInput)
	{
		// 해당 스킬의 캐스팅 시간 소모
		OwnerPlayer->GetWorldTimerManager().SetTimer(CastingEHandle, this, &USkillComponent::CastingEndESkil, SkillECastingTime, false);
		bSkillEInput = false;

		if (bSkillUltiCasting)
		{
			DeSpawnBladeStorm();
			bWasUsingUlti = true;
		}
	}

	if (bSkillCInput)
	{	
		if (bSkillUltiCasting)
		{
			DeSpawnBladeStorm();
			bWasUsingUlti = true;
		}
		if (!bHoldingC && !ControlledCloudburst)
		{
			OwnerPlayer->GetWorldTimerManager().SetTimer(CastingCHandle, this, &USkillComponent::CastingEndCSkil, SkillCCastingTime, false);
			bSkillCInput = false;
		}
	}

	// 모든 스킬 사용이 끝나고 궁극기 사용중이였다면
	if (!bUsingSkill && bWasUsingUlti)
	{
		RespawnBladestormAfterUsingSkill();
	}
	else if (!bUsingSkill && bDeactivatedPlayerHands && !bSkillUltiCasting)
	{
		PermitPlayerSwap();
	}
}


void UJettSkillComponent::UseSkillQ()
{
	// 스킬이 남아있지 않으면 사용 불가
	if (SkillQCount <= 0)
	{
		DontHaveSkill();
		return;
	}

	APlayerController* PC = Cast<APlayerController>(OwnerPlayer->GetController());
	if (PC)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), UpdraftSound, OwnerPlayer->GetActorLocation());

		FRotator ControllerRotation = PC->GetControlRotation();
		FRotator YawRotation(0, ControllerRotation.Yaw, 0);
		FVector UpDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Z);
		UpDirection.Normalize();

		if (UCharacterMovementComponent* MovementComponent = OwnerPlayer->GetCharacterMovement())
		{
			float DashSpeed = 800.f;
			FVector LaunchDirection = UpDirection * DashSpeed;

			OwnerPlayer->LaunchCharacter(LaunchDirection, false, true);
			MovementComponent->AirControl = 1.f;

			FTimerHandle DashHandle;

			OwnerPlayer->GetWorldTimerManager().SetTimer(DashHandle, [this, MovementComponent]
				{
					if (MovementComponent)
					{
						MovementComponent->AirControl = 0.05f;
					}
				}, 0.3f, false);

		}

		ProhibitPlayerSwap();
		bSkillQInput = true;
		bSkillQCasting = true;
		bUsingSkill = true;
		SkillQCount--;
	}


	UE_LOG(LogTemp, Display, TEXT("Use Jett Skill Q"));
}

void UJettSkillComponent::UseSkillE()
{
	// 스킬이 남아있지 않으면 사용 불가
	if (SkillECount <= 0)
	{
		DontHaveSkill();
		return;
	}

	// 순풍 활성화
	if (!bTailWind)
	{
		bTailWind = true;
		bActiveHasDurationSkill = true;
		CurrentActiveDurationTime = SkillEDurationTime;
		return;
	}

	// 딜레이 이후 다시 누르면
	if (CurrentActiveAccTime > 0.3f && bTailWind)
	{		
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), DashSound, OwnerPlayer->GetActorLocation());

		bTailWind = false;
		CurrentActiveAccTime = 0.f;
		CurrentActiveDurationTime = 0.f;
		bActiveHasDurationSkill = false;

		// 순풍 매커니즘
		APlayerController* PC = Cast<APlayerController>(OwnerPlayer->GetController());
		if (PC)
		{
			FVector DashDirection = FVector::ZeroVector;

			FRotator ControlRotation = PC->GetControlRotation();
			FRotator YawRotation(0, ControlRotation.Yaw, 0);

			// 카메라의 전방과 오른쪽 방향을 구한다.
			FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			// 키입력에 따라 대쉬방향 결정
			if (PC->IsInputKeyDown(EKeys::W))
			{
				DashDirection += ForwardDirection;
			}
			if (PC->IsInputKeyDown(EKeys::S))
			{
				DashDirection -= ForwardDirection;
			}
			if (PC->IsInputKeyDown(EKeys::D))
			{
				DashDirection += RightDirection;
			}
			if (PC->IsInputKeyDown(EKeys::A))
			{
				DashDirection -= RightDirection;
			}
			if (DashDirection == FVector::ZeroVector)
			{
				// 아무런 키도 없다면 전방으로
				DashDirection += ForwardDirection;
			}

			if (!DashDirection.IsNearlyZero())
			{
				DashDirection.Normalize();

				float DashSpeed = 4000.f;

				// 캐릭터에게 즉각적인 속도 부여
				FVector LaunchVelocity = DashDirection * DashSpeed;
				OwnerPlayer->LaunchCharacter(LaunchVelocity, true, false);

				if (UCharacterMovementComponent* MovementComponent = OwnerPlayer->GetCharacterMovement())
				{
					FTimerHandle DashTimer;

					// 일정 시간이 지나면 (대쉬 끝) 감속
					OwnerPlayer->GetWorldTimerManager().SetTimer(DashTimer, [this, MovementComponent]()
						{
							if (MovementComponent && OwnerPlayer)
							{
								FVector CuurentVelocity = MovementComponent->Velocity;

								CuurentVelocity.X *= 0.2f;
								CuurentVelocity.Y *= 0.2f;

								MovementComponent->Velocity = CuurentVelocity;
							}
						}, 0.3f, false);
				}
			}
		}

		ProhibitPlayerSwap();
		bSkillEInput = true;
		bSkillECasting = true;
		bUsingSkill = true;
		SkillECount--;
		UE_LOG(LogTemp, Display, TEXT("Use Jett Skill E"));
	}
}

void UJettSkillComponent::UseSkillC()
{
	// 스킬이 남아있지 않으면 사용 불가
	if (SkillCCount <= 0)
	{
		DontHaveSkill();
		return;
	}

	bHoldingC = true;

	APlayerController* PC = Cast<APlayerController>(OwnerPlayer->GetController());
	if (PC)
	{
		FVector CameraLocation;
		FRotator CameraRotation;
		PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector FireDirection = CameraRotation.Vector();
		FireDirection.Normalize();

		FVector Spawnpoint = CameraLocation + (FireDirection * 100.f);

		ControlledCloudburst = GetWorld()->SpawnActor<ACloudburst>(CloudburstClass, Spawnpoint, CameraRotation);

		ProhibitPlayerSwap();
		bSkillCInput = true;
		bSkillCCasting = true;
		bUsingSkill = true;
		SkillCCount--;

		UE_LOG(LogTemp, Display, TEXT("Use Jett Skill C"));
	}
}

void UJettSkillComponent::ReleaseSKillC()
{
	bHoldingC = false;

	// 조종 종료
	if (ControlledCloudburst)
	{
		ControlledCloudburst->FinishControl();
		ControlledCloudburst = nullptr;
	}

	// 기존 캐스팅 시간 이상 조종했다면
	if (ControlledTime > SkillCCastingTime)
	{
		// 이전 상태가 궁극기 사용중 이였음 (칼날폭풍 활성화)
		if (bWasUsingUlti)
		{
			RespawnBladestormAfterUsingSkill();
		}
		// 일반 무기
		else
		{
			PermitPlayerSwap();
		}

		bSkillCCasting = false;
		UE_LOG(LogTemp, Display, TEXT("Permit Swap with Rlease Key"));
	}

	ControlledTime = 0.f;
}

void UJettSkillComponent::UseSkillUlti()
{
	// 아직 다 안쓴 칼날폭풍이 있다면
	if (ActivatedBladestormsCount > 0)
	{
		OwnerPlayer->RequestHideHands(true);
		SpawnBladestorm();
	}
	else
	{
		// 궁극기 포인트가 부족하면 사용 불가
		if (CurrentUltimateCount < NeedUltimateCount)
		{
			NeedMoreSkill();
			return;
		}
		
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), DrawBladestormSound, OwnerPlayer->GetActorLocation());

		OwnerPlayer->RequestHideHands(true);
		InitialSpawnBladestorm();
	}


	UE_LOG(LogTemp, Display, TEXT("Use Jett Skill Ulti"));
}

void UJettSkillComponent::UseSkillPassive()
{
	APlayerController* PC = Cast<APlayerController>(OwnerPlayer->GetController());
	if (PC)
	{
		if (UCharacterMovementComponent* MovementComponent = OwnerPlayer->GetCharacterMovement())
		{
			bool bShoudActivate = PC->IsInputKeyDown(EKeys::SpaceBar) && MovementComponent->IsFalling() && MovementComponent->Velocity.Z < 0.f;

			// 패시브 시작
			if (bShoudActivate)
			{
				if (!bPassiveActive)
				{
					bPassiveActive = true;
					MovementComponent->GravityScale = 0.2f;
					MovementComponent->AirControl = 1.f;

					if (FlyStartSound)
					{
						FlyStartAudioComponent = UGameplayStatics::SpawnSoundAttached(FlyStartSound, OwnerPlayer->GetRootComponent());

						if (FlyStartAudioComponent)
						{
							//FlyStartAudioComponent->Play();
							FlyStartAudioComponent->OnAudioFinished.AddDynamic(this, &UJettSkillComponent::OnFlyStartSoundFinished);
						}
					}

				}
			}
			else
			{
				if (bPassiveActive)
				{
					bPassiveActive = false;

					MovementComponent->GravityScale = 1.f;
					MovementComponent->AirControl = 0.05f;

					if (FlyPersistAudioComponent)
					{
						FlyPersistAudioComponent->Stop();
						FlyPersistAudioComponent = nullptr;
					}

					if (FlyStartAudioComponent)
					{
						FlyStartAudioComponent->Stop();
						FlyStartAudioComponent = nullptr;
					}

				}
				
			}
		}
	}
}

void UJettSkillComponent::PressMouseLBTN()
{
	// 플레이어가 칼날폭풍을 장착 중
	if (bSkillUltiCasting)
	{
		if (GetWorld()->GetTimerManager().IsTimerActive(FireTimerHandle))
		{
			return;
		}

		FireBladeStormOnce();

		// 새로운 칼날 폭풍 소환 후 발사
		GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &UJettSkillComponent::FireBladeStormOnce, BladeStormFireDelay, true);
	}
}

void UJettSkillComponent::PressMouseRBTN()
{
	if (bSkillUltiCasting)
	{
		FireBladeStormAll();
	}
}

void UJettSkillComponent::ReleaseMouseLBTN()
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

void UJettSkillComponent::ReleaseMouseRBTN()
{
}

void UJettSkillComponent::CheckPlayerKeyInput(FKey PressedKey)
{
	if (!bSkillUltiCasting)
		return;

	if (PressedKey == EKeys::One)
	{
		DeSpawnBladeStorm();
		bSkillUltiCasting = false;
		OwnerPlayer->SetTemporaryWeaponIdx(0);
		OwnerPlayer->RequestRevealHands(true);
		bDeactivatedPlayerHands = false;		
	}
	else if (PressedKey == EKeys::Two)
	{
		DeSpawnBladeStorm();
		bSkillUltiCasting = false;
		OwnerPlayer->SetTemporaryWeaponIdx(1);
		OwnerPlayer->RequestRevealHands(true);
		bDeactivatedPlayerHands = false;
	}
}

void UJettSkillComponent::OnBladeKillSuccess()
{
	InitialSpawnBladestorm();
}

void UJettSkillComponent::UpdateCloudburstDirection()
{
	APlayerController* PC = Cast<APlayerController>(OwnerPlayer->GetController());

	if (PC)
	{
		FVector CameraLocation;
		FRotator CameraRotation;
		PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector NewDirection = CameraRotation.Vector();

		// 카메라의 방향에따라 위치 업데이트
		ControlledCloudburst->SetHomingDirection(NewDirection);
	}
}

void UJettSkillComponent::FireBladeStormOnce()
{
	if (ActivatedBladestormsCount <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
		return;
	}

	APlayerController* PC = Cast<APlayerController>(OwnerPlayer->GetController());
	if (PC)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ShotBladestormSound, OwnerPlayer->GetActorLocation());

		FVector CameraLocation;
		FRotator CameraRotation;
		PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector FireDirection = CameraRotation.Vector();
		FireDirection.Normalize();

		FVector Spawnpoint = CameraLocation + (FireDirection * 100.f);
		FRotator NewRotation = CameraRotation;
		NewRotation.Pitch += 200.f;
		NewRotation.Roll += 180.f;

		// 칼날 폭풍을 발사한 액터 설정
		SpawnProjectileBladestorm(FireDirection, Spawnpoint, NewRotation, true);
	}
}

void UJettSkillComponent::FireBladeStormAll()
{
	APlayerController* PC = Cast<APlayerController>(OwnerPlayer->GetController());
	if (PC)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), ShotAllBladestormSound, OwnerPlayer->GetActorLocation());

		FVector CameraLocation;
		FRotator CameraRotation;
		PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector FireDirection = CameraRotation.Vector();
		FireDirection.Normalize();

		FRotator NewRotation = CameraRotation;
		NewRotation.Pitch += 200.f;
		NewRotation.Roll += 180.f;

		float SpreadYaw = 0.25f;
		float SpreadPitch = 0.75f;

		int Count = ActivatedBladestormsCount;

		for (int i = 0; i < Count; ++i)
		{
			float IndexOffset = i - (Count - 1) * 0.5;

			FRotator Rot = CameraRotation;
			Rot.Yaw += IndexOffset * SpreadYaw;
			Rot.Pitch += FMath::RandRange(-SpreadPitch, SpreadPitch);

			FVector Dir = Rot.Vector();
			FVector SpawnPoint = CameraLocation + Dir * 100.f;

			SpawnProjectileBladestorm(Dir, SpawnPoint, NewRotation, false);
		}
	}
}

void UJettSkillComponent::SpawnProjectileBladestorm(const FVector& FireDirection, const FVector& SpawnLocation, const FRotator& SpanwRotation, bool SingleFire)
{
	// 칼날 폭풍을 발사한 액터 설정
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerPlayer;
	SpawnParams.Instigator = OwnerPlayer->GetInstigator();

	ABladeStorm* NewBladestorm = GetWorld()->SpawnActor<ABladeStorm>(BladestormClass, SpawnLocation, SpanwRotation, SpawnParams);	
	DeSpawnOneBladeStorm();

	NewBladestorm->SetOwner(OwnerPlayer);	
	NewBladestorm->SetAttached(false);
	NewBladestorm->SetProjectile(FireDirection, SpanwRotation);
	NewBladestorm->SetSingleFired(SingleFire);
}


FTransform UJettSkillComponent::GetBladeTransformForView(int Index)
{
	if (!OwnerPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("OwnerPlayer is NULL"));
		return FTransform::Identity;
	}
	APlayerController* PC = Cast<APlayerController>(OwnerPlayer->GetController());
	if (!PC)
	{
		return FTransform::Identity;
	}

	FVector CameraLocation;
	FRotator CameraRotation;

	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector Forward = CameraRotation.Vector();
	FVector Right = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Y);
	FVector Up = FRotationMatrix(CameraRotation).GetUnitAxis(EAxis::Z);

	FVector Offset;
	switch (Index)
	{
	case 0:
		Offset = Forward * 55 + Right * 50 + Up * 90;
		break;
	case 1:
		Offset = Forward * 55 + Right * -50 + Up * 90;
		break;
	case 2:
		Offset = Forward * 55 + Right * 35 + Up * 80;
		break;
	case 3:
		Offset = Forward * 55 + Right * -35 + Up * 80;
		break;
	case 4:
		Offset = Forward * 50 + Right * 0 + Up * 70;
		break;
	}

	FTransform Result;
	Result.SetLocation(CameraLocation + Offset);
	return Result;
}

void UJettSkillComponent::InitialSpawnBladestorm()
{
	for (int i = 0; i < Bladestorms.Num(); ++i)
	{
		Bladestorms[i]->SetActorHiddenInGame(false);
		Bladestorms[i]->SetSpanwed(true);
	}
	ActivatedBladestormsCount = 5;
	bSkillUltiCasting = true;
}

void UJettSkillComponent::SpawnBladestorm()
{
	for (int i = 0; i < Bladestorms.Num(); ++i)
	{
		if (Bladestorms[i]->IsSpawned())
		{
			Bladestorms[i]->SetActorHiddenInGame(false);
		}
	}
	bSkillUltiCasting = true;
}

void UJettSkillComponent::RespawnBladestormAfterUsingSkill()
{
	for (int i = 0; i < Bladestorms.Num(); ++i)
	{
		if (Bladestorms[i]->IsSpawned())
		{
			Bladestorms[i]->SetActorHiddenInGame(false);
		}
	}
	bSkillUltiCasting = true;
	bWasUsingUlti = false;
	OwnerPlayer->SetCanSwapWeapon(true);

	UE_LOG(LogTemp, Display, TEXT("Respawned Bladestrom !!!!!"));
}

void UJettSkillComponent::DeSpawnBladeStorm()
{
	for (int i = 0; i < Bladestorms.Num(); ++i)
	{
		Bladestorms[i]->SetActorHiddenInGame(true);
	}
	bSkillUltiCasting = false;

	UE_LOG(LogTemp, Display, TEXT("Despawned Bladestorm !!"));
}

void UJettSkillComponent::DeSpawnOneBladeStorm()
{
	for (int i = 0; i < Bladestorms.Num(); ++i)
	{
		if (!Bladestorms[i]->IsHidden())
		{
			// 한개의 칼날폭풍을 비활성화 했다면 return
			Bladestorms[i]->SetActorHiddenInGame(true);
			Bladestorms[i]->SetSpanwed(false);
			ActivatedBladestormsCount--;
			if (ActivatedBladestormsCount <= 0)
			{
				ActivatedBladestormsCount = 0;
				CurrentUltimateCount = 0;
				EndSkillUlti();				
			}
			return;
		}
	}
}

void UJettSkillComponent::OnFlyStartSoundFinished()
{
	if (bPassiveActive && FlyPersistSound)
	{
		FlyPersistAudioComponent = UGameplayStatics::SpawnSoundAttached(FlyPersistSound, OwnerPlayer->GetRootComponent());

		if (FlyPersistAudioComponent)
		{
			FlyPersistAudioComponent->bIsUISound = false;
			FlyPersistAudioComponent->Play();
		}
	}

}

void UJettSkillComponent::EndSkillUlti()
{
	bSkillUltiCasting = false;
	OwnerPlayer->RequestRevealHands(true);
	bDeactivatedPlayerHands = false;
}
