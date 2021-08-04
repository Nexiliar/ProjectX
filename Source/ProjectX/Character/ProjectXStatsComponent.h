// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectX/Game/ProjectXGameInstance.h"
#include "ProjectX/Character/ProjectXHealthComponent.h"
#include "ProjectX/Character/ProjectXInventoryComponent.h"
#include "ProjectX/Character/ProjectXSkillComponent.h"
#include "ProjectX/FunctionLibrary/Types.h"
#include "ProjectXStatsComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStatsChange, FStatsInfo,CharStats,int32,CharacterLevel,int32,CurrentAttributePoints);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelUp);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTX_API UProjectXStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProjectXStatsComponent();

	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Statistics")
		FOnStatsChange OnStatsChange;
	UPROPERTY(BlueprintAssignable, EditAnywhere, BlueprintReadWrite, Category = "Statistics")
		FOnLevelUp OnLevelUp;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	int32 CurrentLevel = 1;	
	int32 AttributePoints = 0;
	float CurrentExpirience = 0.0f;
	float LevelExpirience;

	
	FStatsInfo CharacterStatistic;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Expirience")
		TArray<float>  AmountOfExpirienceNeedForLvlUpPerLevel;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
		int32 MaxLevel = 30;
	UProjectXHealthComponent* HealthComponent;
	UProjectXInventoryComponent* Inventory;
	
	
		bool React10 = false;
		bool React15 = false;
		bool React20 = false;

		bool Con10 = false;
		bool Con15 = false;
		bool Con20 = false;


	//Function to gain expirience for level up
	UFUNCTION(BlueprintCallable, Category = "Expirience")
		void ExpGain(float Amount);
	//Function to count how much exp do you have and witch level are you. Also how many exp you should collect to levelup
	UFUNCTION(BlueprintCallable, Category = "Expirience")
		float GetExpInfo(float& CurrentMaxExp);
	UFUNCTION(BlueprintCallable)
		UProjectXHealthComponent*  GetHealthComponent();
	
	UFUNCTION(BlueprintCallable, Category = "Expirience")
		void LevelUpEvent();
	

	UFUNCTION(BlueprintCallable, Category = "Attribute")
		bool UseAttributePoints(int32 PointsDoDecrease);

	UFUNCTION(BlueprintCallable, Category = "SkillPoint")
		bool RiseStat(EStatTypesName StatName, int32 AmountOfPoits);
	UFUNCTION(BlueprintCallable, Category = "SkillPoint")
		void ConRiseResult(bool Init, int32 AmountOfPointstoAdd);
	UFUNCTION(BlueprintCallable, Category = "SkillPoint")
		void StrRiseResult(bool Init,int32 AmountOfPointstoAdd);
	UFUNCTION(BlueprintCallable, Category = "SkillPoint")
		bool IsSlotUnlocked(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "SkillPoint")
		void ReactionRiseResult(bool Init,int32 AmountOfPointstoAdd);
	UFUNCTION(BlueprintCallable, Category = "Info")
		FStatsInfo GetEveryStat(int32& level, int32& att);
	UFUNCTION(BlueprintCallable, Category = "Init")
		void StatsInit(int32 level, int32 att, FStatsInfo stats);
	UFUNCTION(Category = "BasicInit")
		void BasicCompsInit();

	/* Delete if not used
	UFUNCTION(BlueprintCallable, Category = "SkillPoint")
		FStatsInfo GetCharacterInfo(int32& CurrentSkillPoints, int32& CurrentAttributePoints, int32& CharacterLevel);
*/

};
