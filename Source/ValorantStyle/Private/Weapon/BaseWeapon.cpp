// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/BaseWeapon.h"
#include "Player/ValorantPlayer.h"


// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	Mesh->bCastDynamicShadow = false;
	Mesh->CastShadow = false;
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (WeaponState != EWeaponState::Firing)
	{
		CurrentSpread = FMath::FInterpTo(CurrentSpread, 0.f, DeltaTime, SpreadRecoverSpeed);
		AccumulatedAimOffset = FMath::Vector2DInterpTo(AccumulatedAimOffset, FVector2D::ZeroVector, DeltaTime, 30);
	}	
}

bool ABaseWeapon::TryReload()
{
	// 이미 재장전
	if (WeaponState == EWeaponState::Reloading)
		return false;

	if (!CanReload())
		return false;

	WeaponState = EWeaponState::Reloading;
	OnReloadStart();
	return true;
}

void ABaseWeapon::SetPullTrigger(bool Pull)
{
	bPullTrigger = Pull;
}

void ABaseWeapon::StartReload()
{

}

void ABaseWeapon::EndReload()
{

}
void ABaseWeapon::AllAnimEndReload()
{

}

EWeaponState ABaseWeapon::GetCurWeaponState() const
{
	return WeaponState;
}

bool ABaseWeapon::CanReload() const
{	
	return curAmmo < maxAmmo && 0 < leftAmmo;
}

void ABaseWeapon::OnReloadStart()
{

}

void ABaseWeapon::FireOnce()
{

}

bool ABaseWeapon::GunTraceWithRecoil(FHitResult& Hit, FVector& ShotDirection, const FVector2D& Recoil)
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return false;
	}
	AController* OwnerController = OwnerPawn->GetController();
	if (!OwnerController)
	{
		return false;
	}

	FVector Location;
	FRotator Rotation;

	// 사용자의 ViewPoint의 위치와 회전값을 얻어 사격의 방향과, 끝점을 구한다.
	OwnerController->GetPlayerViewPoint(Location, Rotation);
	
	// 현재 적용할 Recoil 패턴을 적용시킨다. (에임 기준점을 올린다 -> 크로스헤어를 벗어나 탄착군 형성 -> 스프레이 잡기 컨트롤)
	Rotation.Pitch += AccumulatedAimOffset.Y;
	Rotation.Yaw += Recoil.X;

	// 현재 적용해야 할 탄퍼짐 정도를 구해 원뿔구조로 흩뿌린다.
	FVector ShotDir;
	
	if (IsUnstableState())
	{
		float SpreadAngle = GetCurrentSpread();
		ShotDir = FMath::VRandCone(Rotation.Vector(), FMath::DegreesToRadians(SpreadAngle));
	}
	else
	{
		ShotDir = Rotation.Vector();
	}

	ShotDirection = -ShotDir;
	FVector End = Location + ShotDir * MaxRange;
	FCollisionQueryParams Params;
	// 해당 무기와 사용자는 충돌 무시
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	return GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECC_GameTraceChannel2, Params);
}

FVector2D ABaseWeapon::GetCurrentRecoil() const
{
	if (SprayPattern.Num() == 0)
	{
		return FVector2D::ZeroVector;
	}

	// 현재 무기의 스프레이 패턴에서 가져온다
	int32 Index = FMath::Min(CurrentShotIndex, SprayPattern.Num() - 1);
	FVector2D Recoil = SprayPattern[Index];

	// 약간의 미세 랜덤 반동 추가
	Recoil.X += FMath::RandRange(-0.1f, 01.f);
	Recoil.Y += FMath::RandRange(0.f, 0.1f);

	Recoil *= GetRecoilMultiplier();

	return Recoil;
}

float ABaseWeapon::GetRecoilMultiplier() const
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return 1.f;
	}

	AValorantPlayer* Player = Cast<AValorantPlayer>(OwnerPawn);
	if (!Player) return 1.f;

	float Multiplier = BaseRecoilMultiplier;
	
	// 플레이어의 상태에따라 반동 값을 추가로 더해준다.
	if (Player->IsCrouched())
	{
		Multiplier *= CrouchRecoilMultiplier;
	}

	if (Player->IsMoving())
	{
		Multiplier *= MoveRecoilMultiplier;
	}

	return Multiplier;
}

float ABaseWeapon::GetCurrentSpread() const
{
	float Spread = BaseSpread;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	AValorantPlayer* Player = Cast<AValorantPlayer>(OwnerPawn);

	if (Player)
	{
		if (Player->IsMoving())
		{
			Spread += MoveSpread;
		}

		if (Player->IsCrouched())
		{
			Spread *= 0.7f;
		}
	}

	Spread += CurrentSpread;

	return FMath::Min(Spread, MaxSpread);
}

bool ABaseWeapon::IsUnstableState() const
{
	AValorantPlayer* Player = Cast<AValorantPlayer>(GetOwner());

	if (!Player)
	{
		return true;
	}

	return Player->IsMoving() || Player->IsAirborne();
}

AController* ABaseWeapon::GetOwnerController() const
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return nullptr;
	}
	return OwnerPawn->GetController();
}
