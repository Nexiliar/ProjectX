// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectX/Game/GameObj/DestructionEnviroment_Structure.h"
#include "Materials/MaterialInterface.h"
#include "PhysicalMaterials/PhysicalMaterial.h"


// Sets default values
ADestructionEnviroment_Structure::ADestructionEnviroment_Structure()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	MainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh "));
	MainMesh->SetupAttachment(RootComponent);

	EffectSpawnSphereLocation = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EffectSpawn"));
	EffectSpawnSphereLocation->SetupAttachment(MainMesh);
	

	HealthComponent = CreateDefaultSubobject<UProjectXHealthComponent>(TEXT("HealthComponent"));
}

// Called when the game starts or when spawned
void ADestructionEnviroment_Structure::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADestructionEnviroment_Structure::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

EPhysicalSurface ADestructionEnviroment_Structure::GetSurfaceType()
{
	EPhysicalSurface Result = EPhysicalSurface::SurfaceType_Default;
	UStaticMeshComponent* myMesh = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));

	if (myMesh)
	{
		UMaterialInterface* MyMaterial = myMesh->GetMaterial(0);
		if (MyMaterial)
		{
			Result = MyMaterial->GetPhysicalMaterial()->SurfaceType;
		}
	}
	return Result;
}

TArray<UProjectX_StateEffect*> ADestructionEnviroment_Structure::GetAllCurrentEffects()
{
	return Effects;
}

void ADestructionEnviroment_Structure::RemoveEffect(UProjectX_StateEffect* RemoveEffect)
{

	Effects.Remove(RemoveEffect);
}

void ADestructionEnviroment_Structure::AddEffect(UProjectX_StateEffect* NewEffect)
{
	Effects.Add(NewEffect);
}

void ADestructionEnviroment_Structure::GetSpawnLocationForEffect(FVector& MeshLocation, FName& BoneName)
{
	//UStaticMeshComponent* myMesh = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (MainMesh)
	{
		MeshLocation = EffectSpawnSphereLocation->GetRelativeLocation();
		BoneName = "None";
	}
}

