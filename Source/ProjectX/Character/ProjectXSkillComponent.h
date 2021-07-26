// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetArrayLibrary.h"
#include "ProjectX/Character/ProjectXHealthComponent.h"
#include "ProjectX/FunctionLibrary/Types.h"
#include "Components/ActorComponent.h"
#include "ProjectXSkillComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimerStarted, float, TimerTick);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBonusTimerStarted, float, TimerTick);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSkillSwitched);

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
	
	//CurrentSkill
	FTimerHandle TimerHandle_SlowMoTimer;
	FTimerHandle TimerHandle_CoolDownTimer;
	FTimerHandle TimerHandle_CheckTimerValue;
	
	//BonusSkill	
	FTimerHandle TimerHanlde_BonusSkillCooldown;
	FTimerHandle TimerHandle_CheckBonusTimerValue;
	
	//Rage
	FTimerHandle TimerHandle_RageTimer;

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FOnTimerStarted OnTimerStarted;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FOnBonusTimerStarted OnBonusTimerStarted;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Inventory")
		FOnSkillSwitched OnSkillSwitched;
	
	bool isSkillOnCoolDown = false;
	UPROPERTY(BlueprintReadWrite, Category = "CoolDown")
		bool isBonusSkillOnCoolDown = false;

	UPROPERTY(BlueprintReadOnly,Category = "Skill Config")
		float CoolDown;
	UPROPERTY(BlueprintReadOnly, Category = "Skill Config")
		float BonusSkillCoolDown;
	UPROPERTY(EditAnywhere, Category = "Skill Config")
		float RageModeTimer = 70;

	UPROPERTY(BlueprintReadOnly, Category = "Skill Config")
		float TimerForWIdgetUpdateInfo;
	UPROPERTY(BlueprintReadOnly, Category = "Skill Config")
		float BonusTimerForWIdgetUpdateInfo;
	
	//SkillSlots
	UPROPERTY(BlueprintReadWrite, Category = "Skills")
		ESkillList CurrentSkill = ESkillList::SlowMode;
	UPROPERTY(BlueprintReadWrite, Category = "Skills")
		ESkillList BonusSkill = ESkillList::RageMode;
	UFUNCTION(BlueprintCallable, Category = "Skills")
		void SwitchSkills();
	UFUNCTION(BlueprintCallable, Category = "Skills")
		void ChoseSkill(ESkillList Skill);
	

	UFUNCTION(Category = "Skill Config")
		void SkillIsEnable();
	UFUNCTION(Category = "Skill Config")
		void CheckTimeRemainingOnCoolDown();
	UFUNCTION(Category = "Skill Config")
		void CheckTimeRemainingOnBonusSkillCoolDown();

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
	
	//RageSkill
	UPROPERTY(EditAnywhere, Category = "RageSkill")
		UParticleSystem* RageStart = nullptr;
	UPROPERTY(EditAnywhere, Category = "RageSkill")
		USoundBase* RageSound = nullptr;
	UPROPERTY(BlueprintReadWrite, Category = "RageSkill")
		bool isRageModeAvailable = false;
	UPROPERTY(BlueprintReadWrite, Category = "RageSkill")
		bool isRageModeOn = false;
	UFUNCTION(BlueprintCallable, Category = "RageSkill")
		void RageMode();
	UFUNCTION(BlueprintCallable, Category = "RageSkill")
		void RageModeEnd();

	//SnakeMode
	UPROPERTY(BlueprintReadWrite, Category = "SnakeMode")
		bool isSnakeModeAvailable = false;
	
	//BastionMode
	UPROPERTY(BlueprintReadWrite, Category = "BastionMode")
		bool isBastionModeAvailable = false;
	UPROPERTY(EditAnywhere, Category = "BastionMode")
		UParticleSystem* BastionParticle = nullptr;
	UPROPERTY(EditAnywhere, Category = "BastionMode")
		USoundBase* BastionSound = nullptr;
	UFUNCTION(BlueprintCallable, Category ="BastionMode")
		void BastionMode();
	UFUNCTION(BlueprintCallable, Category = "BastionMode")
		void BastionModeEnd();
};
