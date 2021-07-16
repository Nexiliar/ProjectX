// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "ProjectXHealthComponent.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChange,float,Health, float, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDead);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTX_API UProjectXHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProjectXHealthComponent();

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Health")
		FOnHealthChange OnHealthChange;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Health")
		FOnDead OnDead;

	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	float MaxHealth;
	float Health;

public:	
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float CoefDamage = 1.0f;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable, Category = "Health")
		float GetCurrentHealth();
	UFUNCTION(BlueprintCallable, Category = "Health")
		float GetCurrentMaxHealth();

	UFUNCTION(BlueprintCallable, Category = "Health")
		void SetCurrentHealth(float NewHealth);

	UFUNCTION(BlueprintCallable, Category = "Health")
		void SetCurrentMaxHealth(float ChangeValue);

	UFUNCTION(BlueprintCallable, Category = "Health")
		virtual void ChangeHealthValue(float ChangeValue);
	
	//UFUNCTION(BlueprintCallable, Category = "Health")
		//virtual void ChangeHealthValue(float ChangeValue);

		
};
