// Fill out your copyright notice in the Description page of Project Settings.


#include "Bot/DummyBot.h"
#include "Bot/BotSpawner.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ADummyBot::ADummyBot()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADummyBot::BeginPlay()
{
	Super::BeginPlay();
	
	BotSpawner = Cast<ABotSpawner>(UGameplayStatics::GetActorOfClass(GetWorld(), ABotSpawner::StaticClass()));
	InitBoneMap();
	Health = 100.f;		
}

// Called every frame
void ADummyBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADummyBot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool ADummyBot::IsDead() const
{
	return Health <= 0;
}

float ADummyBot::GetHPPercent() const
{
	return Health / MaxHealth;
}


float ADummyBot::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// 적용할 데미지
	float TotalDamageToApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	TotalDamageToApplied = CalculatePartialDamage(TotalDamageToApplied, DamageEvent);
	UE_LOG(LogTemp, Display, TEXT("Applied Damage : %f"), TotalDamageToApplied);
	
	// 현재 체력과 쉴드에 비례하여 체력을 감소 시킨다.
	CalculateDamageWithShield(TotalDamageToApplied);
	UE_LOG(LogTemp, Warning, TEXT("Helath left : %f"), Health);
	UE_LOG(LogTemp, Warning, TEXT("Shield left : %f"), Shield);

	if (IsDead())
	{
		DummyDeathProcess();
	}

	return TotalDamageToApplied;
}

void ADummyBot::InitBoneMap()
{
	BoneToPartMap.Add("head", EHitPart::Head);

	BoneToPartMap.Add("upperarm_l", EHitPart::ArmLeg);
	BoneToPartMap.Add("lowerarm_l", EHitPart::ArmLeg);
	BoneToPartMap.Add("hand_l", EHitPart::ArmLeg);

	BoneToPartMap.Add("upperarm_r", EHitPart::ArmLeg);
	BoneToPartMap.Add("lowerarm_r", EHitPart::ArmLeg);
	BoneToPartMap.Add("hand_R", EHitPart::ArmLeg);

	BoneToPartMap.Add("thigh_l", EHitPart::ArmLeg);
	BoneToPartMap.Add("calf_l", EHitPart::ArmLeg);
	BoneToPartMap.Add("foot_l", EHitPart::ArmLeg);

	BoneToPartMap.Add("thigh_r", EHitPart::ArmLeg);
	BoneToPartMap.Add("calf_r", EHitPart::ArmLeg);
	BoneToPartMap.Add("foot_r", EHitPart::ArmLeg);
}

float ADummyBot::CalculatePartialDamage(float OriginDamage, FDamageEvent const& DamageEvent)
{
	FName HitBoneName = NAME_None;

	if (DamageEvent.GetTypeID() == FPointDamageEvent::ClassID)
	{
		const FPointDamageEvent* PointDamageEvent = static_cast<const FPointDamageEvent*>(&DamageEvent);
		HitBoneName = PointDamageEvent->HitInfo.BoneName;
		UE_LOG(LogTemp, Warning, TEXT("Hit Bone is : %s"), *HitBoneName.ToString());
	}

	EHitPart Part = BoneToPartMap.FindRef(HitBoneName);
	switch (Part)
	{
	case EHitPart::Head:
		OriginDamage *= 4.f;
		break;
	case EHitPart::Body:
		break;
	case EHitPart::ArmLeg:
		OriginDamage *= 0.85f;
		break;
	case EHitPart::None:
		break;
	default:
		break;
	}

	return OriginDamage;
}

void ADummyBot::CalculateDamageWithShield(float OriginDamage)
{
	float DamageToHealth = OriginDamage;

	// 쉴드가 있는 경우
	if (Shield > 0)
	{
		// 데미지의 일정량은 쉴드에 적용시킨다.
		float DamageToShield = DamageToHealth * 0.8;
		DamageToHealth -= DamageToShield;

		// 남은 데미지를 체력에 적용할 데미지에 합쳐준다.
		Shield -= DamageToShield;
		if (Shield < 0)
		{
			DamageToHealth -= Shield;
		}

		// 만약 체력에 적용시킬 데미지가 현재 체력보다 더 큰데 쉴드가 남아있을 경우
		if (DamageToHealth > Health && Shield > 0)
		{
			// 체력을 1남기고 나머지 데미지를 쉴드데미지로 환산
			DamageToShield = DamageToHealth - (Health - 1);
			DamageToHealth -= DamageToShield;

			Shield -= DamageToShield;
			if (Shield < 0)
			{
				DamageToHealth -= Shield;
			}
		}
	}

	// 체력 감소
	Health -= DamageToHealth;
}

void ADummyBot::DummyDeathProcess()
{
	UE_LOG(LogTemp, Display, TEXT("Bot Is Dead"));
	// 게임에서 비활성화
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);

	// 타이머 핸들 초기화
	GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);

	// 일정 시간후 새로운 랜덤한 위치에서 다시 생성시킨다.
	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this,  &ADummyBot::DummyRespawnWithNewLocation,0.2f, false);
}

void ADummyBot::DummyRespawnWithNewLocation()
{
	FVector NewLocation;

	if (BotSpawner->GiveNewSpawnLocation(NewLocation))
	{
		UE_LOG(LogTemp, Display, TEXT("Success"));
		SetActorLocation(NewLocation);
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
		Health = 100.f;
		if (bUseShield)
		{
			Shield = 50.f;
		}
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Success"));
	}
}


void ADummyBot::OnShield(bool bMax)
{
	if (bMax)
		Shield = 50.f;
	else
		Shield = 25.f;

	bUseShield = true;
}

void ADummyBot::OffShield()
{
	Shield = 0.f;
	bUseShield = false;
}
