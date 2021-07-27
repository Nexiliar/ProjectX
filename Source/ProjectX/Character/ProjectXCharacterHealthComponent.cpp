// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectX/Character/ProjectXCharacterHealthComponent.h"

void UProjectXCharacterHealthComponent::ChangeHealthValue(float ChangeValue)
{
	float CurrentDamage = ChangeValue * CoefDamage;
	
	//Health = Health + ChangeValue;
	if (Shield > 0.0f && ChangeValue < 0.0f)
	{
		ChangeShieldValue(ChangeValue);
		if (Shield < 0.0f)
		{
			//FX
			UE_LOG(LogTemp, Warning, TEXT(" UProjectXCharacterHealthComponent::ChangeHealthValue - Shield<0"));
		}
	}
	else
	{
		Super::ChangeHealthValue(ChangeValue);
	}

}

float UProjectXCharacterHealthComponent::GetCurrentShield()
{
	return Shield;
}

float UProjectXCharacterHealthComponent::GetMaxShield()
{
	return MaxShield;
}

void UProjectXCharacterHealthComponent::ChangeShieldValue(float ChangeValue)
{
	Shield += ChangeValue;
	OnShieldChange.Broadcast(Shield, ChangeValue);

	if (Shield > MaxShield)
	{
		Shield = MaxShield;
	}
	else
	{
		if (Shield < 0.0f)		
			Shield = 0.0f;
		
	}
	/* 
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_CoolDownShieldTimer, this, &UProjectXCharacterHealthComponent::CoolDownShieldEnd, CoolDownShieldRcoverTime, false);
		
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ShieldTimerRecoveryRateTimer);
	}
	*/
	
}
void UProjectXCharacterHealthComponent::SetShieldValue(float ChangeValue)
{
	MaxShield = ChangeValue;
	Shield = ChangeValue;
	OnShieldChange.Broadcast(Shield, ChangeValue);
}
/* 
void UProjectXCharacterHealthComponent::CoolDownShieldEnd()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ShieldTimerRecoveryRateTimer, this, &UProjectXCharacterHealthComponent::RecoveryShield, ShieldRecoveryRate, true);
	}

}*/

/* void UProjectXCharacterHealthComponent::RecoveryShield()
{
	float tmp = Shield;
	tmp = tmp + ShieldRecoveryValue;
	OnShieldChange.Broadcast(Shield, ShieldRecoveryValue);
	if (tmp > 100.0f)
	{
		Shield = 100.0f;
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ShieldTimerRecoveryRateTimer);
		}
	}
	else
	{
		Shield = tmp;
	}
	
}*/

void UProjectXCharacterHealthComponent::HealthRegenStart(float HealthToRestore)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_HealthRecoveryTimer, this, &UProjectXCharacterHealthComponent::HealthRegen, HealthRegenSpeed, true);
	}
}

void UProjectXCharacterHealthComponent::HealthRegen()
{
	if (TmpVariableForHealth < AmountOfHealthRestoredByMedKit && Health < MaxHealth)
	{
		TmpVariableForHealth += 1;
		Health += 1;
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_HealthRecoveryTimer);
		TmpVariableForHealth = 0;
		
	}
	OnHealthChange.Broadcast(Health, 1.0f);

}
