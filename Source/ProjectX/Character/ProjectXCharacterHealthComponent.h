// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectX/Character/ProjectXHealthComponent.h"
#include "ProjectXCharacterHealthComponent.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnShieldChange, float, Shield, float, Damage);

UCLASS()
class PROJECTX_API UProjectXCharacterHealthComponent : public UProjectXHealthComponent
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Shield")
		FOnShieldChange OnShieldChange;

	FTimerHandle TimerHandle_CoolDownShieldTimer;
	FTimerHandle TimerHandle_ShieldTimerRecoveryRateTimer;
	FTimerHandle TimerHandle_HealthRecoveryTimer;

protected:
	float MaxShield = 0.0f;
	float Shield = 0.0f;

public:
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
		float CoolDownShieldRcoverTime = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
		float ShieldRecoveryValue = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shield")
		float ShieldRecoveryRate = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float HealthRegenSpeed = 0.1f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		int32 AmountOfHealthRestoredByMedKit = 20;
		int32 TmpVariableForHealth = 0;


	//ќверрайд функции, UFunction не нужен
	void ChangeHealthValue(float ChangeValue) override;
	UFUNCTION(BlueprintCallable, Category = "Health")
	void HealthRegenStart(float HealthToRestore);

	void HealthRegen();
	UFUNCTION(BlueprintCallable, Category = "Shield")
	float GetCurrentShield();
	UFUNCTION(BlueprintCallable, Category = "Shield")
		float GetMaxShield();
	
	void ChangeShieldValue(float ChangeValue);

	UFUNCTION(BlueprintCallable, Category = "Shield")
		void SetShieldValue(float ChangeValue);

	//void CoolDownShieldEnd();
	
	//void RecoveryShield();
};
