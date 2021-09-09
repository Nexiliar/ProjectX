// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Particles/ParticleSystemComponent.h"
#include "ProjectX_StateEffect.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDebuffTimerStarted, float, TimeRemainingForWidget, float, AdditionalTimeRemainingForWidget);
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
		bool isInterrupted = false;
	

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

UCLASS(Blueprintable, meta = (ShowWorldContextPin))
class PROJECTX_API UProjectX_StateEffect_ExecuteTimer : public UProjectX_StateEffect
{
	GENERATED_BODY()

		// Перезаписываем функцию для вызова после инициализации.
		virtual void PostInitProperties() override;
public:
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Debuff")
		FOnDebuffTimerStarted OnDebuffTimeStarted;
	bool InitObject(AActor* Actor) override;
	void DestroyObject() override;

	UFUNCTION(BlueprintPure)
		AActor* GetOwner() const { return Cast<AActor>(GetOuter()); };
	
	// Функция, которую определяем мы уже в Блупринтах.
	UFUNCTION(BlueprintImplementableEvent)
		void BeginPlay();

	virtual void Execute();
	
	

	void TimeRemaining();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting ExecuteTimer")
		float Power = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting ExecuteTimer")
		float Timer = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting ExecuteTimer")
		float RateTime = 1.0f;
	UPROPERTY(BlueprintReadOnly, Category = "Debuff Timer For Widget")
		float WidgetDebuffTimer = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Additional Debuff Timer For Widget")
		float AdditionalWidgetDebuffTimer = 0.0f;

	FTimerHandle TimerHandle_ExecuteTimer;
	FTimerHandle TimerHandle_EffectTimer;
	FTimerHandle TimerHandle_WidgetInfo;
	FTimerHandle TimerHandle_AdditionalExecuteTimer;
	FTimerHandle TimerHandle_AdditionalEffectTimer;
	FTimerHandle TimerHandle_AdditionalWidgetInfo;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Setting ExecuteTimer")
		UParticleSystem* ParticleEffect = nullptr;
	
	UParticleSystemComponent* ParticleEmitter = nullptr;
	
};

