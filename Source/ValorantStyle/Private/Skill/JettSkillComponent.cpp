// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/JettSkillComponent.h"
#include "Player/ValorantPlayer.h"
#include "Skill/Object/Cloudburst.h"
#include "Skill/Object/BladeStorm.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Camera/CameraComponent.h"

UJettSkillComponent::UJettSkillComponent()
{
	static ConstructorHelpers::FClassFinder<ACloudburst> CloudburstFinder(TEXT("/Game/BP_Cloudburst"));
	static ConstructorHelpers::FClassFinder<ABladeStorm> BladestormFinder(TEXT("/Game/BP_Bladestorm"));


	if (CloudburstFinder.Succeeded())
	{
		CloudburstClass = CloudburstFinder.Class;
	}

	if (BladestormFinder.Succeeded())
	{
		BladestormClass = BladestormFinder.Class;
	}

}

void UJettSkillComponent::BeginPlay()
{
	bPassive = true;

	SkillQCount = 5;
	SkillECount = 5;
	SkillCCount = 5;
	NeedUltimateCount = 7;
	CurrentUltimateCount = 7;

	SkillQCastingTime = 1.f;
	SkillECastingTime = 1.f;
	SkillCCastingTime = 0.1f;
}

void UJettSkillComponent::CharacterSelected()
{
	// 제트 칼날 폭풍 미리 소환
	for (int i = 0; i < 5; ++i)
	{
		ABladeStorm* NewBladestorm = GetWorld()->SpawnActor<ABladeStorm>(BladestormClass);

		NewBladestorm->AttachToComponent(OwnerPlayer->GetCameraComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		FTransform NewTransform = GetBladeTransformForView(i);
		NewTransform.SetScale3D(NewBladestorm->GetActorScale3D());
		NewBladestorm->SetActorTransform(NewTransform);
		NewBladestorm->SetActorRelativeRotation(FRotator(200.f, 0.f, 180.f));
		NewBladestorm->SetInitialRelativeLocation();

		NewBladestorm->SetActorHiddenInGame(true);
		NewBladestorm->SetActorEnableCollision(false);
		NewBladestorm->SetAttached(true);

		Bladestorms.Add(NewBladestorm);
	}
}


void UJettSkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (bTailWind)
	{
		AccTime += DeltaTime;

		// 일정 시간안에 스킬을 다시 발동시키지 않으면 초기화
		if (AccTime > 10.f)
		{
			bTailWind = false;
			SkillECount--;
		}
		UE_LOG(LogTemp, Display, TEXT("%f"), AccTime);
	}

	// C키를 지속적으로 누른상태라면 (한번도 떼지 않고)
	if (bHoldingC && ControlledCloudburst)
	{
		ControlledTime += DeltaTime;
		UpdateCloudburstDirection();
	}

	if (bSkillQCasting)
	{
		StartPermitTimer(bSkillQCasting);
		bSkillQCasting = false;
	}

	if (bSkillECasting)
	{
		StartPermitTimer(bSkillECasting);
		bSkillECasting = false;
	}

	if (bSkillCCasting)
	{
		if (!bHoldingC && !ControlledCloudburst)
		{
			StartPermitTimer(bSkillCCasting);
			bSkillCCasting = false;
		}
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
		bSkillQCasting = true;
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
		return;
	}

	// 딜레이 이후 다시 누르면
	if (AccTime > 0.3f && bTailWind)
	{
		// 순풍 매커니즘
		AccTime = 0.f;
		bTailWind = false;

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
			if(DashDirection == FVector::ZeroVector)
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
		bSkillECasting = true;
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
		bSkillCCasting = true;
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
		PermitPlayerSwap();
		bSkillCCasting = false;
		UE_LOG(LogTemp, Display, TEXT("Permit Swap with Rlease Key"));
	}
	
	ControlledTime = 0.f;
}

void UJettSkillComponent::UseSkillUlti()
{
	// 궁극기 포인트가 부족하면 사용 불가
	if (CurrentUltimateCount < NeedUltimateCount)
	{
		NeedMoreSkill();
		return;
	}

	OwnerPlayer->SetHideHands(true);
	bSkillUltiCasting = true;
	SpawnBladestorm();

	UE_LOG(LogTemp, Display, TEXT("Use Jett Skill Ulti"));
}

void UJettSkillComponent::UseSkillPassive()
{
	APlayerController* PC = Cast<APlayerController>(OwnerPlayer->GetController());
	if (PC)
	{
		if (UCharacterMovementComponent* MovementComponent = OwnerPlayer->GetCharacterMovement())
		{
			if (PC->IsInputKeyDown(EKeys::SpaceBar))
	{
				if (MovementComponent->IsFalling() && MovementComponent->Velocity.Z < 0.f)
				{
					MovementComponent->GravityScale = 0.2f;
					MovementComponent->AirControl = 1.f;
				}
			}
			else
			{
				MovementComponent->GravityScale = 1.f;
				MovementComponent->AirControl = 0.05f;
			}
		}
	}
}

void UJettSkillComponent::PressMouseLBTN()
{
	// 플레이어가 칼날폭풍을 장착 중
	//if (OwnerPlayer->IsHasBladestorm())
	//{
	//	// 새로운 칼날 폭풍 소환 후 발사
	//	//GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &UJettSkillComponent::FireBladeStormOnce, BladeStormFireDelay, true);
	//}
}

void UJettSkillComponent::PressMouseRBTN()
{
	//if (OwnerPlayer->IsHasBladestorm())
	//{
	//	FireBladeStormAll();
	//}
}

void UJettSkillComponent::CheckPlayerKeyInput(FKey PressedKey)
{
	if (!bSkillUltiCasting)
		return;

	if (PressedKey == EKeys::One || PressedKey == EKeys::Two)
	{
		DeSpawnBladeStorm();
		bSkillUltiCasting = false;
		OwnerPlayer->SetRevealHands(true);
		OwnerPlayer->SetChangeWeaponWhenReveal(true);
	}
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
	APlayerController* PC = Cast<APlayerController>(OwnerPlayer->GetController());
	if (PC)
	{
		FVector CameraLocation;
		FRotator CameraRotation;
		PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector FireDirection = CameraRotation.Vector();
		FireDirection.Normalize();

		FVector Spawnpoint = CameraLocation + (FireDirection * 100.f);
		GetWorld()->SpawnActor<ACloudburst>(CloudburstClass, Spawnpoint, CameraRotation);

	}
}

void UJettSkillComponent::FireBladeStormAll()
{
}


FTransform UJettSkillComponent::GetBladeTransformForView(int Index)
{
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
		Offset = Forward * 55 + Right * 35 + Up * 80;
		break;
	case 1:
		Offset = Forward * 55 + Right * -35 + Up * 80;
		break;
	case 2:
		Offset = Forward * 50 + Right * 0 + Up * 70;
		break;
	case 3:
		Offset = Forward * 55 + Right * 50 + Up * 90;
		break;
	case 4:
		Offset = Forward * 55 + Right * -50 + Up * 90;
		break;
	}

	FTransform Result;
	Result.SetLocation(CameraLocation + Offset);
	return Result;
}

void UJettSkillComponent::SpawnBladestorm()
{
	for (int i = 0; i < Bladestorms.Num(); ++i)
	{
		Bladestorms[i]->SetActorHiddenInGame(false);
	}
}

void UJettSkillComponent::DeSpawnBladeStorm()
{
	for (int i = 0; i < Bladestorms.Num(); ++i)
	{
		Bladestorms[i]->SetActorHiddenInGame(true);
	}
}