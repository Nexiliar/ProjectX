// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectX/Weapons/Projectiles/ProjectileDefault.h"
#include "MyProjectileDefault_Grenade1.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTX_API AMyProjectileDefault_Grenade1 : public AProjectileDefault
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void TimerExplose(float DeltaTime);

	virtual void BulletCollisionSphereHit(class UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void ImpactProjectile() override;

	void Explose();

	void BombExplosion();

	bool TimerEnabled = false;
	
	
	float TimerToExplose = 0.0f;
	//CustomBomb
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BombCustomize")
		float TimeToExplose = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BombCustomize")
		bool isABomb = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BombCustomize")
		UParticleSystem* ExplodeFX = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BombCustomize")
		USoundBase* ExplodeSound = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BombCustomize")
		float MaxRadius = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BombCustomize")
		float MinRadius = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BombCustomize")
		float ExloseDamage = 0.0f;

};
