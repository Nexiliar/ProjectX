// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectX/Character/ProjectXHealthComponent.h"

// Sets default values for this component's properties
UProjectXHealthComponent::UProjectXHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	// ...
}


// Called when the game starts
void UProjectXHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	
	// ...
	
}


// Called every frame
void UProjectXHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UProjectXHealthComponent::GetCurrentHealth()
{
	return Health;
}

float UProjectXHealthComponent::GetCurrentMaxHealth()
{
	return MaxHealth;
}


void UProjectXHealthComponent::SetCurrentHealth(float NewHealth)
{
	Health = NewHealth;
	OnHealthChange.Broadcast(Health,0.0f);
}

void UProjectXHealthComponent::SetCurrentMaxHealth(float ChangeValue)
{
	MaxHealth = ChangeValue;
	Health = MaxHealth;
	OnHealthChange.Broadcast(Health, 0);
}

void UProjectXHealthComponent::ChangeHealthValue(float ChangeValue)
{
	ChangeValue = ChangeValue * CoefDamage;
	Health = Health + ChangeValue;
	OnHealthChange.Broadcast(Health, ChangeValue);
	
	if (Health > MaxHealth)
	{
		Health = MaxHealth;
	}
	else
	{
		if (Health <= 0.0f)
		{
			OnDead.Broadcast();

		}
	}
	
}

void UProjectXHealthComponent::AddMaxHealthValue(float NewHealth)
{
	MaxHealth += NewHealth;
	OnHealthChange.Broadcast(Health, 0);
}


