// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectX/Weapons/Projectiles/ProjectileDefault_Grenade.h"

// Sets default values
AProjectileDefault_Grenade::AProjectileDefault_Grenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AProjectileDefault_Grenade::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectileDefault_Grenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

