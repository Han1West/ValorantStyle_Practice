#include "C:\Users\Hanwonseo\Documents\Unreal Projects\ValorantStyle\Intermediate\Build\Win64\x64\ValorantStyleEditor\Development\Engine\SharedPCH.Engine.Cpp20.InclOrderUnreal5_0.h"
// Fill out your copyright notice in the Description page of Project Settings.


#include "Bot/DummyBot.h"
#include "Components/CapsuleComponent.h"

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
		TotalDamageToApplied *= 4.f;
		break;
	case EHitPart::Body:
		break;
	case EHitPart::ArmLeg:
		TotalDamageToApplied *= 0.85f;
		break;
	case EHitPart::None:
		break;
	default:
		break;
	}

	UE_LOG(LogTemp, Display, TEXT("Applied Damage : %f"), TotalDamageToApplied);

	// 현재 체력과 쉴드에 비례하여 체력을 감소 시킨다.
	float DamageToHealth = TotalDamageToApplied;

	// 쉴드가 있는 경우
	if (Shield > 0)
	{
		// 데미지의 일정량은 쉴드에 적용시킨다.
		float DamageToShield = TotalDamageToApplied * 0.8;
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
	
	UE_LOG(LogTemp, Warning, TEXT("Helath left : %f"), Health);
	UE_LOG(LogTemp, Warning, TEXT("Shield left : %f"), Shield);

	if (IsDead())
	{
		// 만약 죽었다면 충돌체 비활성화
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		UE_LOG(LogTemp, Display, TEXT("Bot Is Dead"));
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
