// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/JettSkillComponent.h"
#include "Player/ValorantPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"

void UJettSkillComponent::BeginPlay()
{
	bPassive = true;

	SkillQCount = 5;
	SkillECount = 5;
	SkillCCount = 2;
	NeedUltimateCount = 7;
	CurrentUltimateCount = 0;
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
	if (AccTime > 0.5f && bTailWind)
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

	SkillCCount--;

	UE_LOG(LogTemp, Display, TEXT("Use Jett Skill C"));
}

void UJettSkillComponent::UseSkillUlti()
{
	// 궁극기 포인트가 부족하면 사용 불가
	if (CurrentUltimateCount <= NeedUltimateCount)
	{
		NeedMoreSkill();
		return;
	}

	CurrentUltimateCount = 0;


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
					UE_LOG(LogTemp, Display, TEXT("Use Passive"));
				}
			}
			else
			{
				MovementComponent->GravityScale = 1.f;
				MovementComponent->AirControl = 0.05f;
				UE_LOG(LogTemp, Display, TEXT("do not Use Passive"));
			}
		}
	}
}