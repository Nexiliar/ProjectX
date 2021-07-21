// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetArrayLibrary.h"
#include "ProjectX/FunctionLibrary/Types.h"
#include "Components/ActorComponent.h"
#include "ProjectXSkillComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimerStarted, float, TimerTick);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTX_API UProjectXSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProjectXSkillComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FTimerHandle TimerHandle_SlowMoTimer;
	FTimerHandle TimerHandle_CoolDownTimer;
	FTimerHandle TimerHandle_CheckTimerValue;

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FOnTimerStarted OnTimerStarted;
	bool isSkillOnCoolDown = false;
	UPROPERTY(BlueprintReadOnly,Category = "Skill Config")
		float CoolDown;
	//UPROPERTY(EditAnywhere, Category = "Skill Config")
	//	float RecallCoolDown = 3;
	//UPROPERTY(EditAnywhere, Category = "Skill Config")
	//	float SlowMoCoolDown = 3;

	UPROPERTY(BlueprintReadOnly, Category = "Skill Config")
		float TimerForWIdgetUpdateInfo;

	

	UFUNCTION(Category = "Skill Config")
		void SkillIsEnable();
	UFUNCTION(Category = "Skill Config")
		void CheckTimeRemainingOnCoolDown();
	UFUNCTION(Category = "Skill Config")
		void InitTimerRemainingCooldown();

	//TeleportSkill
	UPROPERTY(EditAnywhere, Category = "Teleport")
		UParticleSystem* StartTeleportPoint = nullptr;
	UPROPERTY(EditAnywhere, Category = "Teleport")
		UParticleSystem* EndTeleportPoint = nullptr;
	UPROPERTY(EditAnywhere, Category = "Teleport")
		USoundBase* TeleportSound = nullptr;
	UFUNCTION(BlueprintCallable, Category = "Teleport")
		void Teleport(float TeleportCoolDown);
	//SlowModeSkill
	UPROPERTY(EditAnywhere, Category = "SlowMode")
		float SlowModeTime = 5.0f;
	UFUNCTION(BlueprintCallable, Category = "SlowMode")
		void SlowMode(float SlowMoCooldown, float TimerRemain);
	UFUNCTION(BlueprintCallable, Category = "SlowMode")
		void SlowModeEnd();

	//RecallSkill
	UPROPERTY(EditAnywhere, Category = "Recall")
		UParticleSystem* StartRecallPoint = nullptr;
	UPROPERTY(EditAnywhere, Category = "Recall")
		UParticleSystem* EndRecallPoint = nullptr;
	UPROPERTY(EditAnywhere, Category = "Recall")
		USoundBase* RecallSound = nullptr;
	UFUNCTION(BlueprintCallable, Category = "Recall")
		void Recall(float RecallCoolDown);
	
		
};
