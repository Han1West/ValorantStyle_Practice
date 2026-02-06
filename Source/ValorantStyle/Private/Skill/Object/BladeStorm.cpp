// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/Object/BladeStorm.h"
#include "Player/ValorantPlayer.h"
#include "Skill/JettSkillComponent.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ABladeStorm::ABladeStorm()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BladeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BladeMesh"));
	RootComponent = BladeMesh;

	BladeMesh->SetNotifyRigidBodyCollision(true);
	BladeMesh->SetGenerateOverlapEvents(true);
	
	BladeMesh->OnComponentHit.AddDynamic(this, &ABladeStorm::OnBladeHit);

	ProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	ProjectileComponent->UpdatedComponent = BladeMesh;		
	ProjectileComponent->InitialSpeed = 0.f;
	ProjectileComponent->MaxSpeed = 4000.f;
	ProjectileComponent->bRotationFollowsVelocity = false;
	ProjectileComponent->bInitialVelocityInLocalSpace = false;
	ProjectileComponent->SetAutoActivate(true);	


	SetActorScale3D(FVector(0.1f, 0.1f, 0.1f));
}

// Called when the game starts or when spawned
void ABladeStorm::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called every frame
void ABladeStorm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bAttached)
	{
		AccTime += DeltaTime;

		// 지속적인 흔들림
		FVector Shake;
		Shake.X = FMath::Sin(AccTime * ShakeSpeed) * ShakeStrength;
		Shake.Y = FMath::Cos(AccTime * ShakeSpeed * 1.3f) * ShakeStrength;
		Shake.Z = FMath::Sin(AccTime * ShakeSpeed * 0.7f) * ShakeStrength;

		BladeMesh->SetRelativeLocation(InitialRelativeLocation + Shake);
	}
}

void ABladeStorm::SetInitialRelativeLocation()
{
	InitialRelativeLocation = BladeMesh->GetRelativeLocation();
}

void ABladeStorm::SetProjectile(const FVector& FireDirection, const FRotator& Rotation)
{
	SetActorEnableCollision(true);

	ProjectileComponent->Activate(true);	
	FVector Dir = FireDirection.GetSafeNormal();

	ProjectileComponent->Velocity = ProjectileComponent->MaxSpeed * Dir;
	ProjectileComponent->UpdateComponentVelocity();	
}

void ABladeStorm::NotifyKill(AActor* Victim)
{
	if (AValorantPlayer* Player = Cast<AValorantPlayer>(GetOwner()))
	{
		if (UJettSkillComponent* Skill = Cast<UJettSkillComponent>(Player->GetSkillComponent()))
		{
			if (bSingleFired)
			{
				Skill->OnBladeKillSuccess();
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed TO Cast SkillComponent"));
		}
	}
}

void ABladeStorm::OnBladeHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this)
	{
		UE_LOG(LogTemp, Display, TEXT("Hit event Return"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Blade Hit : %s"), *OtherActor->GetName());
	
	FVector ShotDirection = ProjectileComponent->Velocity.GetSafeNormal();
	FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
	AController* OwnerController = nullptr;

	if (AValorantPlayer* OwnerPlayer = Cast<AValorantPlayer>(GetOwner()))
	{
		OwnerController = OwnerPlayer->GetController();
	}

	OtherActor->TakeDamage(Damage, DamageEvent, OwnerController, this);

	if(OtherActor)

	Destroy();
}

