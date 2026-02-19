// Fill out your copyright notice in the Description page of Project Settings.


#include "Skill/Object/Cloudburst.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACloudburst::ACloudburst()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere"));
	RootComponent = MeshComponent;

	MeshComponent->SetNotifyRigidBodyCollision(true);

	ProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile"));

	ProjectileComponent->UpdatedComponent = MeshComponent;

	ProjectileComponent->InitialSpeed = 2000.f;
	ProjectileComponent->MaxSpeed = 2000.f;
}

// Called when the game starts or when spawned
void ACloudburst::BeginPlay()
{
	Super::BeginPlay();
	
	if (!IsValid(MeshComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("MeshComponent Missing!"));
		return;
	}
	MeshComponent->OnComponentHit.AddDynamic(this, &ACloudburst::OnHit);
}

void ACloudburst::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), BurstSound, GetActorLocation());

	bActivated = true;

	FVector NewScale = MeshComponent->GetComponentScale();
	NewScale *= 5.f;

	MeshComponent->SetWorldScale3D(NewScale);
	UE_LOG(LogTemp, Warning, TEXT("Hit Event!"));
}

// Called every frame
void ACloudburst::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bActivated)
	{
		AccTime += DeltaTime;
	}

	// 생명 시간 끝나면 사라짐
	if(LifeTime < AccTime)
	{
		AccTime = 0.f;
		bActivated = false;
		Destroy();
	}
}

void ACloudburst::SetHomingDirection(FVector Direction)
{
	if (!ProjectileComponent)
	{
		return;
	}

	// 부드럽게 회전하기 위해 보간
	FVector Current = ProjectileComponent->Velocity;
	FVector Target = Direction * ProjectileComponent->InitialSpeed;

	FVector Blended = FMath::VInterpTo(Current, Target, GetWorld()->DeltaTimeSeconds, 5.f);

	ProjectileComponent->Velocity = Blended;
}

void ACloudburst::FinishControl()
{
	bIsControlled = false;
}

