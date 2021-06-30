// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Particles/ParticleSystemComponent.h"
#include "ProjectX_StateEffect.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class PROJECTX_API UProjectX_StateEffect : public UObject
{
	GENERATED_BODY()
	
public:
	
	virtual bool InitObject(AActor* Actor);
	virtual void DestroyObject();
	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	TArray<TEnumAsByte<EPhysicalSurface>> PossibleInteractSurface;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
		bool bIsStackable = false;

	AActor* myActor = nullptr;
};

UCLASS()
class PROJECTX_API UProjectX_StateEffect_ExecuteOnce : public UProjectX_StateEffect
{
	GENERATED_BODY()
public:
	bool InitObject(AActor* Actor) override;
	void DestroyObject() override;

	virtual void ExecuteOnce();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category ="Setting ExecuteOnce")
		float Power = 20.0f;
};

UCLASS()
class PROJECTX_API UProjectX_StateEffect_ExecuteTimer : public UProjectX_StateEffect
{
	GENERATED_BODY()
public:
	
	bool InitObject(AActor* Actor) override;
	void DestroyObject() override;
	


	virtual void Execute();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting ExecuteTimer")
		float Power = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting ExecuteTimer")
		float Timer = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting ExecuteTimer")
		float RateTime = 1.0f;

	FTimerHandle TimerHandle_ExecuteTimer;
	FTimerHandle TimerHandle_EffectTimer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting ExecuteTimer")
		UParticleSystem* ParticleEffect = nullptr;
	
	UParticleSystemComponent* ParticleEmitter = nullptr;
	
};