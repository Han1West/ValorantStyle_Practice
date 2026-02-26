// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/PrimaryGun.h"
#include "Magazine/Magazine.h"
#include "Player/ValorantPlayer.h"

#include "DrawDebugHelpers.h"
#include "Components/DecalComponent.h"
#include "kismet/GameplayStatics.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

// Sets default values
APrimaryGun::APrimaryGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APrimaryGun::BeginPlay()
{
	Super::BeginPlay();

	maxAmmo = 25;
	curAmmo = 25;
	curleftAmmo = 75;
	maxleftAmmo = 75;
	bUseAmmo = true;

	TriggerDelayTime = 0.1f;

	MaxRange = 10000.f;
	NormalDamage = 35.f;
	

	CurrentMagazine = GetWorld()->SpawnActor<AMagazine>(MagazineClass);
	CurrentMagazine->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform, TEXT("Magazine_Socket"));
}

// Called every frame
void APrimaryGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (WeaponState != EWeaponState::Reloading)
	{
		if (IsHidden())
		{
			CurrentMagazine->SetActorHiddenInGame(true);
		}
		else
		{
			CurrentMagazine->SetActorHiddenInGame(false);
		}
	}
}

// 탄창 제거
void APrimaryGun::StartReload()
{
	if (!CurrentMagazine) return;

	CurrentMagazine->SetActorHiddenInGame(true);
	
}

// 탄창 생성 및 총알 업데이트
void APrimaryGun::EndReload()
{
	if (!CurrentMagazine) return;

	CurrentMagazine->SetActorHiddenInGame(false);

	int32 needAmmo = maxAmmo - curAmmo;
	int32 usedAmmo = FMath::Min(needAmmo, curleftAmmo);

	curAmmo += usedAmmo;
	curleftAmmo -= usedAmmo;
}

void APrimaryGun::StopReload()
{
	if (!CurrentMagazine) return;

	CurrentMagazine->SetActorHiddenInGame(false);

	AllAnimEndReload();
}

void APrimaryGun::AllAnimEndReload()
{
	WeaponState = EWeaponState::Idle;

	// 재장전 하는동안 방아쇠를 계속 누르고 있다면 바로 발사 로직을 수행
	if (bPullTrigger)
	{
		StartFire();
	}
}

void APrimaryGun::SetPullTrigger(bool Pull)
{
	// 재장전 중에는 발사 입력을 무시한다
	if (WeaponState == EWeaponState::Reloading)
	{
		bPullTrigger = Pull;
		return;
	}

	// 첫 방아쇠 (딜레이 없는 첫발 발사)
	if (Pull && !bPullTrigger)
	{
		StartFire();
	}
	// 방아쇠를 놓음
	else if (!Pull && bPullTrigger)
	{
		StopFire();
	}

	Super::SetPullTrigger(Pull);
}

void APrimaryGun::OnReloadStart()
{
	StopFire();
}


bool APrimaryGun::CanFire() const
{
	return curAmmo > 0;
}

void APrimaryGun::StartFire()
{
	if (!CanFire()) return;

	WeaponState = EWeaponState::Firing;

	FireOnce();

	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &APrimaryGun::FireOnce, TriggerDelayTime, true);
}

void APrimaryGun::StopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);

	if (WeaponState == EWeaponState::Firing)
	{
		WeaponState = EWeaponState::Idle;
	}
}

void APrimaryGun::FireOnce()
{
	if (!CanFire())
	{
		StopFire();
		return;
	}

	UGameplayStatics::SpawnSoundAttached(AttackSound, Mesh, TEXT("Muzzle_Socket"));

	float CurrentTime = GetWorld()->TimeSeconds;
	// 만약 회복 시간을 지나면 패턴 초기화
	if (CurrentTime - LastFireTime > RecoilResetTime)
	{
		CurrentShotIndex = 0;
	}
	
	// 현재에 발사 탄퍼짐을 적용
	CurrentSpread += FireSpread;
	CurrentSpread = FMath::Clamp(CurrentSpread, 0.f, MaxSpread);

	// 현재 적용시킬 반동 가져오기
	FVector2D Recoil = GetCurrentRecoil();
	// 현재 반동이 적용된 Offset을 기억한다
	CurrentRecoilOffset = Recoil;

	AccumulatedAimOffset.Y += Recoil.Y;
	AccumulatedAimOffset.X = 0.f;

	AccumulatedAimOffset.Y = FMath::Min(AccumulatedAimOffset.Y, 100.f);

	// 반동 메쉬에 동시 적용
	FVector RecoilLocation;
	RecoilLocation.X = -Recoil.Y * 0.2f;
	RecoilLocation.Y = Recoil.X * 0.1f;
	RecoilLocation.Z = -Recoil.Y * 0.1f;
	FRotator RecoilRotation;
	RecoilRotation.Pitch = -Recoil.Y * 0.5f;
	RecoilRotation.Yaw = Recoil.X * 0.3f;
	RecoilRotation.Roll = Recoil.X * 0.2f;

	OwnerPlayer->SetHandMeshRelativeLocationRotaiton(RecoilLocation, RecoilRotation);

	FHitResult Hit;
	FVector ShotDirection;
	
	FVector Start = Mesh->GetSocketLocation(TEXT("Muzzle_Socket"));
	FVector End;

	UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleFlashEffect, Mesh, TEXT("Muzzle_Socket"),
		FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);

	if (GunTraceWithRecoil(Hit, ShotDirection, Recoil))
	{
		End = Hit.ImpactPoint;

		FRotator SparkRotation = Hit.ImpactNormal.Rotation();
		FVector SparkLocation = Hit.ImpactPoint + Hit.ImpactNormal * 2.f;

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BulletSparkEffect, SparkLocation, SparkRotation);
	
		
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{			
			// 벽 과 같은 Static 물체에 충돌 시 사운드 재생
			if (HitActor->GetRootComponent()->Mobility == EComponentMobility::Static)
			{
				UGameplayStatics::SpawnSoundAtLocation(GetWorld(), BulletHitSound, Hit.ImpactPoint, FRotator::ZeroRotator, 0.2f);

				UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), BulletHoleMaterial, 
					FVector(10.f, 10.f, 10.f), Hit.ImpactPoint, Hit.ImpactNormal.Rotation(), 5.f);

				if (Decal)
				{
					Decal->SetFadeScreenSize(0.0001f);				
				}
			}

			FPointDamageEvent DamageEvent(NormalDamage, Hit, ShotDirection, nullptr);			
			AController* OwnerController = GetOwnerController();
			HitActor->TakeDamage(NormalDamage, DamageEvent, OwnerController, this);
		}
		
		if (TracerEffect)
		{
			UNiagaraComponent* Tracer =	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TracerEffect, Start);

			if (Tracer)
			{
				Tracer->SetVectorParameter(TEXT("Target"), End);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Line Trace Failed"));
	}

	// 반동 패턴 갱신
	LastFireTime = CurrentTime;
	CurrentShotIndex++;

	Super::FireOnce();
	curAmmo--;
}

