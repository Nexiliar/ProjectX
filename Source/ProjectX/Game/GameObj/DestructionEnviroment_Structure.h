// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectX/Character/ProjectXHealthComponent.h"
#include "ProjectX/Interfaces/ProjectX_Interface_GameActor.h"
#include "ProjectX/Weapons/ProjectX_StateEffect.h"
#include "DestructionEnviroment_Structure.generated.h"

UCLASS()
class PROJECTX_API ADestructionEnviroment_Structure : public AActor, public IProjectX_Interface_GameActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADestructionEnviroment_Structure();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UStaticMeshComponent* MainMesh = nullptr;
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Spawn, meta = (AllowPrivateAccess = "true"))
//		class USphereComponent* EffectSpawnSphereLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = Components)
		class UStaticMeshComponent* EffectSpawnSphereLocation = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UProjectXHealthComponent* HealthComponent;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	EPhysicalSurface GetSurfaceType() override;


	//Effects
	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly)
	TArray<UProjectX_StateEffect*> Effects;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FVector EffectOffset;

	TArray<UProjectX_StateEffect*> GetAllCurrentEffects() override;
	void RemoveEffect(UProjectX_StateEffect* RemoveEffect) override;
	void AddEffect(UProjectX_StateEffect* NewEffect) override;
	void GetSpawnLocationForEffect(FVector& MeshLocation, FName& BoneName) override;
};
