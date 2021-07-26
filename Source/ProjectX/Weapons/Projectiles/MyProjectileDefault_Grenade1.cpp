// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectX/Weapons/Projectiles/MyProjectileDefault_Grenade1.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "MyProjectileDefault_Grenade1.h"

void AMyProjectileDefault_Grenade1::BeginPlay()
{
	Super::BeginPlay();
}

void AMyProjectileDefault_Grenade1::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimerExplose(DeltaTime);
}

void AMyProjectileDefault_Grenade1::TimerExplose(float DeltaTime)
{
	if (TimerEnabled)
	{
		if (TimerToExplose > TimeToExplose)
		{
			if (isABomb)
			{
				BombExplosion();
			}
			else
			{
				Explose();
			}
			
			

		}
		else
		{
			TimerToExplose += DeltaTime;
		}
	}
}

void AMyProjectileDefault_Grenade1::BulletCollisionSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::BulletCollisionSphereHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

void AMyProjectileDefault_Grenade1::ImpactProjectile()
{
	//Init Grenade
	TimerEnabled = true;
}

void AMyProjectileDefault_Grenade1::Explose()
{
	TimerEnabled = false;
	if (ProjectileSetting.ExploseFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileSetting.ExploseFX, GetActorLocation(), GetActorRotation(), FVector(1.0f));
	}
	if (ProjectileSetting.ExploseSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ProjectileSetting.ExploseSound, GetActorLocation());
	}

	TArray<AActor*> IgnoredActor;
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),
		ProjectileSetting.ExploseMaxDamage,
		ProjectileSetting.ExploseMaxDamage * 0.2f,
		GetActorLocation(),
		ProjectileSetting.MinRadius,
		ProjectileSetting.MaxRadius,
		ProjectileSetting.DamageDecayFromCenter,
		NULL, IgnoredActor, this, nullptr);
	
	
	DrawDebugSphere(GetWorld(), GetActorLocation(), ProjectileSetting.MaxRadius, 8, FColor::White, false, 5.0f);
	DrawDebugSphere(GetWorld(), GetActorLocation(), ProjectileSetting.MinRadius, 16, FColor::Red, false, 5.0f);
	this->Destroy();
}

void AMyProjectileDefault_Grenade1::BombExplosion()
{
	TimerEnabled = false;
	if (ExplodeFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeFX, GetActorLocation(), GetActorRotation(), FVector(1.0f));
	}
	if (ExplodeSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplodeSound, GetActorLocation());
	}

	TArray<AActor*> IgnoredActor;
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(),
		ExloseDamage,
		ExloseDamage * 0.3f,
		GetActorLocation(),
		MinRadius,
		MaxRadius,
		ProjectileSetting.DamageDecayFromCenter,
		NULL, IgnoredActor, this, nullptr);


	//DrawDebugSphere(GetWorld(), GetActorLocation(), MaxRadius, 8, FColor::White, false, 5.0f);
	//DrawDebugSphere(GetWorld(), GetActorLocation(), MinRadius, 16, FColor::Red, false, 5.0f);
	this->Destroy();
}
